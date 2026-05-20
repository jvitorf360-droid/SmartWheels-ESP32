// mqtt_manager.cpp
// Implementação do gerenciamento MQTT

#define MQTT_MAX_PACKET_SIZE 512

#include "mqtt_manager.h"
#include "logger.h"
#include "wifi_manager.h"
#include <queue>

// ========== VARIÁVEIS GLOBAIS PARA DIAGNÓSTICO ==========
extern int contadorReinicios;
extern int contadorFalhasMQTT;

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
WiFiClient espClient;
PubSubClient client(espClient);

// Fila de mensagens pendentes
std::queue<MensagemPendente> filaMensagens;

// Controle de tentativas de reconexão
static int tentativasReconexaoMQTT = 0;
static unsigned long ultimaTentativaMQTT = 0;

// ========== FUNÇÕES AUXILIARES INTERNAS ==========
static void callbackMensagem(char* topic, byte* payload, unsigned int length) {
    String mensagem;
    for (unsigned int i = 0; i < length; i++) {
        mensagem += (char)payload[i];
    }
    
    LOG_INFO("Mensagem recebida no tópico: " + String(topic));
    LOG_DEBUG("  Payload: " + mensagem);
    
    if (String(topic).indexOf("rpc") > 0) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, mensagem);
        
        if (!error) {
            String method = doc["method"] | "";
            
            if (method == "reboot") {
                LOG_INFO("Comando de reboot recebido via MQTT");
                delay(1000);
                ESP.restart();
            }
            else if (method == "set_loglevel") {
                int nivel = doc["params"]["nivel"];
                setLogLevel(nivel);
                LOG_INFO("Nível de log alterado para: " + String(nivel));
            }
            else if (method == "test_alert") {
                String tipo = doc["params"]["tipo"] | "";
                if (tipo == "panico") {
                    LOG_INFO("Comando de teste de pânico recebido");
                    enviarAlertaPanico(true);
                }
            }
        }
    }
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS ==========

void inicializarMQTT() {
    client.setServer(mqtt_server.c_str(), mqtt_port.toInt());
    client.setCallback(callbackMensagem);
    LOG_INFO("MQTT inicializado");
    LOG_INFO("  Servidor: " + mqtt_server + ":" + mqtt_port);
}

bool conectarMQTT() {
    if (mqtt_token == "") {
        LOG_ERROR("Token MQTT não configurado");
        return false;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        LOG_ERROR("WiFi não conectado - não é possível conectar MQTT");
        return false;
    }
    
    LOG_INFO("Conectando ao broker MQTT...");
    
    String clientId = "ESP32_SmartWheels_" + getMacAddress();
    
    if (client.connect(clientId.c_str(), mqtt_token.c_str(), "")) {
        LOG_INFO("✅ Conectado ao ThingsBoard via MQTT!");
        tentativasReconexaoMQTT = 0;
        client.subscribe("v1/devices/me/rpc/request/+");
        LOG_DEBUG("Inscrito em tópicos RPC");
        return true;
    } else {
        LOG_ERROR("❌ Falha na conexão MQTT. Estado: " + String(client.state()));
        return false;
    }
}

void desconectarMQTT() {
    if (client.connected()) {
        client.disconnect();
        LOG_INFO("MQTT desconectado");
    }
}

void loopMQTT() {
    if (!client.connected()) {
        unsigned long agora = millis();
        if (agora - ultimaTentativaMQTT > 10000) {
            ultimaTentativaMQTT = agora;
            tentativasReconexaoMQTT++;
            LOG_INFO("Tentativa " + String(tentativasReconexaoMQTT) + " de reconexão MQTT");
            conectarMQTT();
        }
    } else {
        client.loop();
    }
}

void publicarMensagem(String topico, String payload) {
    if (client.connected()) {
        if (client.publish(topico.c_str(), payload.c_str())) {
            LOG_DEBUG("Mensagem publicada com sucesso no tópico: " + topico);
        } else {
            LOG_ERROR("Falha ao publicar mensagem no tópico: " + topico);
        }
    } else {
        LOG_DEBUG("MQTT desconectado - não foi possível publicar");
    }
}

void publicarSeguro(String topico, String payload) {
    if (client.connected()) {
        if (client.publish(topico.c_str(), payload.c_str())) {
            LOG_DEBUG("Mensagem publicada com sucesso");
            return;
        } else {
            LOG_ERROR("Falha ao publicar no tópico: " + topico);
            LOG_ERROR("Payload: " + payload.substring(0, 100));
        }
    }
    
    filaMensagens.push({topico, payload, millis()});
    if (filaMensagens.size() > 50) {
        filaMensagens.pop();
    }
}

void processarFilaMensagens() {
    if (!client.connected()) {
        return;
    }
    
    int enviadas = 0;
    
    while (!filaMensagens.empty() && client.connected() && enviadas < 10) {
        MensagemPendente msg = filaMensagens.front();
        if (client.publish(msg.topico.c_str(), msg.payload.c_str())) {
            filaMensagens.pop();
            enviadas++;
        } else {
            break;
        }
    }
    
    if (enviadas > 0) {
        LOG_DEBUG("Processadas " + String(enviadas) + " mensagens da fila");
    }
    
    static unsigned long ultimoLogErro = 0;
    if (filaMensagens.size() > 0 && millis() - ultimoLogErro > 30000) {
        ultimoLogErro = millis();
        LOG_ERROR("Fila com " + String(filaMensagens.size()) + " mensagens pendentes");
    }
}

// ========== NOVAS FUNÇÕES (payload enxuto com estado) ==========

void enviarAlertaPanico(bool ativo) {
    LOG_INFO("Enviando ALERTA DE PÂNICO - Estado: " + String(ativo ? "ATIVO" : "DESATIVADO"));
    
    Localizacao loc = obterLocalizacao(TIMEOUT_GPS_MS);
    
    if (isnan(loc.latitude) || isnan(loc.longitude)) {
        LOG_ERROR("Localização inválida! Usando valores padrão");
        loc.latitude = SIM_LATITUDE_PADRAO;
        loc.longitude = SIM_LONGITUDE_PADRAO;
    }
    
    String payload = "{";
    payload += "\"botao_acionado\":\"panico\",";
    payload += "\"estado_acionamento\":" + String(ativo ? 1 : 0) + ",";
    payload += "\"latitude\":" + String(loc.latitude, 6) + ",";
    payload += "\"longitude\":" + String(loc.longitude, 6) + ",";
    payload += "\"token\":\"" + mqtt_token + "\"";
    payload += "}";
    
    LOG_INFO("Payload: " + payload);
    LOG_INFO("Tamanho: " + String(payload.length()) + " bytes");
    
    if (client.publish(TOPICO_TELEMETRIA, payload.c_str())) {
        LOG_INFO("✅ Alerta de PÂNICO enviado com sucesso!");
    } else {
        LOG_ERROR("❌ Falha no envio do alerta");
    }
}

void enviarAlertaAcessibilidade(bool ativo) {
    LOG_INFO("Enviando ALERTA DE ACESSIBILIDADE - Estado: " + String(ativo ? "ATIVO" : "DESATIVADO"));
    
    Localizacao loc = obterLocalizacao(TIMEOUT_GPS_MS);
    
    if (isnan(loc.latitude) || isnan(loc.longitude)) {
        LOG_ERROR("Localização inválida! Usando valores padrão");
        loc.latitude = SIM_LATITUDE_PADRAO;
        loc.longitude = SIM_LONGITUDE_PADRAO;
    }
    
    String payload = "{";
    payload += "\"botao_acionado\":\"acessibilidade\",";
    payload += "\"estado_acionamento\":" + String(ativo ? 1 : 0) + ",";
    payload += "\"latitude\":" + String(loc.latitude, 6) + ",";
    payload += "\"longitude\":" + String(loc.longitude, 6) + ",";
    payload += "\"token\":\"" + mqtt_token + "\"";
    payload += "}";
    
    LOG_INFO("Payload: " + payload);
    LOG_INFO("Tamanho: " + String(payload.length()) + " bytes");
    
    if (client.publish(TOPICO_TELEMETRIA, payload.c_str())) {
        LOG_INFO("✅ Alerta de ACESSIBILIDADE enviado com sucesso!");
    } else {
        LOG_ERROR("❌ Falha no envio do alerta");
    }
}

void enviarTelemetria() {
    if (!client.connected()) {
        return;
    }
    
    String payload = "{";
    payload += "\"e\":\"TEL\",";
    payload += "\"ts\":" + String(millis()) + ",";
    payload += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    payload += "\"gps\":" + String(gpsTemSinal() ? "1" : "0") + ",";
    payload += "\"sat\":" + String(getNumeroSatelites()) + ",";
    payload += "\"up\":" + String(millis() / 1000) + ",";
    payload += "\"reinicios\":" + String(contadorReinicios) + ",";
    payload += "\"falhas\":" + String(contadorFalhasMQTT);
    payload += "}";
    
    if (client.publish(TOPICO_TELEMETRIA, payload.c_str())) {
        LOG_DEBUG("Telemetria enviada");
    } else {
        LOG_ERROR("Falha ao enviar telemetria");
    }
}

void enviarHeartbeat() {
    if (!client.connected()) {
        return;
    }
    
    String payload = "{";
    payload += "\"e\":\"HBT\",";
    payload += "\"ts\":" + String(millis()) + ",";
    payload += "\"id\":\"" + getMacAddress() + "\",";
    payload += "\"fw\":\"" + String(VERSAO_FIRMWARE) + "\"";
    payload += "}";
    
    if (client.publish(TOPICO_TELEMETRIA, payload.c_str())) {
        LOG_DEBUG("Heartbeat enviado");
    } else {
        LOG_ERROR("Falha ao enviar heartbeat");
    }
}

bool mqttConectado() {
    return client.connected();
}