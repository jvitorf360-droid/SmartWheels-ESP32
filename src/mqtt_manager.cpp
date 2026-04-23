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

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

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
    // Converte payload para string
    String mensagem;
    for (unsigned int i = 0; i < length; i++) {
        mensagem += (char)payload[i];
    }
    
    LOG_INFO("Mensagem recebida no tópico: " + String(topic));
    LOG_DEBUG("  Payload: " + mensagem);
    
    // Processa comandos recebidos (para implementação futura)
    if (String(topic).indexOf("rpc") > 0) {
        // Comando RPC do ThingsBoard
        JsonDocument doc;
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
                    Localizacao loc = obterLocalizacao(3000);
                    enviarAlertaPanico(loc);
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
    
    // Gera um clientId único baseado no MAC address
    String clientId = "ESP32_SmartWheels_" + getMacAddress();
    
    if (client.connect(clientId.c_str(), mqtt_token.c_str(), "")) {
        LOG_INFO("✅ Conectado ao ThingsBoard via MQTT!");
        tentativasReconexaoMQTT = 0;
        
        // Inscreve em tópicos para receber comandos
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
        // Tenta reconectar a cada 10 segundos
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
            return;
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
            // LOG MAIS DETALHADO AQUI
            LOG_ERROR("Falha ao publicar no tópico: " + topico);
            LOG_ERROR("Payload: " + payload.substring(0, 100)); // primeiros 100 caracteres
        }
    }
    
    // Se falhou ou está desconectado, enfileira
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
    int falhas = 0;
    
    while (!filaMensagens.empty() && client.connected() && enviadas < 10) {
        MensagemPendente msg = filaMensagens.front();
        
        if (client.publish(msg.topico.c_str(), msg.payload.c_str())) {
            filaMensagens.pop();
            enviadas++;
        } else {
            falhas++;
            break;  // Sai do loop se falhar
        }
    }
    
    // Só mostra log se conseguiu enviar algo
    if (enviadas > 0) {
        LOG_DEBUG("Processadas " + String(enviadas) + " mensagens da fila");
    }
    
    // Só mostra erro se a fila estiver cheia (uma vez a cada 30 segundos)
    static unsigned long ultimoLogErro = 0;
    if (filaMensagens.size() > 0 && millis() - ultimoLogErro > 30000) {
        ultimoLogErro = millis();
        LOG_ERROR("Fila com " + String(filaMensagens.size()) + " mensagens pendentes");
    }
}

void enviarAlertaPanico(Localizacao loc) {
    LOG_INFO("Enviando ALERTA DE PÂNICO...");
    
    // Validação da localização
    if (isnan(loc.latitude) || isnan(loc.longitude)) {
        LOG_ERROR("Localização inválida! Usando valores padrão");
        loc.latitude = SIM_LATITUDE_PADRAO;
        loc.longitude = SIM_LONGITUDE_PADRAO;
        loc.precisao = 50;
        loc.fonte = "SIM";
        loc.satelites = 0;
    }
    
    // JSON reduzido (nomes de campos curtos)
    String payload = "{";
    payload += "\"e\":\"PAN\",";
    payload += "\"v\":\"" + String(VERSAO_PAYLOAD) + "\",";
    payload += "\"ts\":" + String(millis()) + ",";
    payload += "\"id\":\"" + getMacAddress() + "\",";
    payload += "\"lat\":" + String(loc.latitude, 6) + ",";
    payload += "\"lng\":" + String(loc.longitude, 6) + ",";
    payload += "\"prec\":" + String(loc.precisao) + ",";
    payload += "\"src\":\"" + loc.fonte + "\",";
    payload += "\"sat\":" + String(loc.satelites) + ",";
    payload += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    payload += "\"bat\":85";
    payload += "}";
    
    LOG_INFO("Payload: " + payload);
    LOG_INFO("Tamanho: " + String(payload.length()) + " bytes");
    
    if (client.publish(TOPICO_TELEMETRIA, payload.c_str())) {
        LOG_INFO("✅ Alerta de PÂNICO enviado com sucesso!");
    } else {
        LOG_ERROR("❌ Falha no envio do alerta");
    }
}

void enviarAlertaAcessibilidade(Localizacao loc) {
    LOG_INFO("Enviando ALERTA DE ACESSIBILIDADE...");
    
    // Validação da localização
    if (isnan(loc.latitude) || isnan(loc.longitude)) {
        LOG_ERROR("Localização inválida! Usando valores padrão");
        loc.latitude = SIM_LATITUDE_PADRAO;
        loc.longitude = SIM_LONGITUDE_PADRAO;
        loc.precisao = 50;
        loc.fonte = "SIM";
        loc.satelites = 0;
    }
    
    // JSON reduzido (nomes de campos curtos)
    String payload = "{";
    payload += "\"e\":\"ACC\",";
    payload += "\"v\":\"" + String(VERSAO_PAYLOAD) + "\",";
    payload += "\"ts\":" + String(millis()) + ",";
    payload += "\"id\":\"" + getMacAddress() + "\",";
    payload += "\"lat\":" + String(loc.latitude, 6) + ",";
    payload += "\"lng\":" + String(loc.longitude, 6) + ",";
    payload += "\"prec\":" + String(loc.precisao) + ",";
    payload += "\"src\":\"" + loc.fonte + "\",";
    payload += "\"sat\":" + String(loc.satelites) + ",";
    payload += "\"rssi\":" + String(WiFi.RSSI()) + ",";
    payload += "\"bat\":85";
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
        incrementarContadorFalhas();
        return;
    }
    
    // JSON reduzido para telemetria com diagnóstico
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
        incrementarContadorFalhas();
    }
}
void enviarHeartbeat() {
    if (!client.connected()) {
        return;
    }
    
    // JSON reduzido para heartbeat
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