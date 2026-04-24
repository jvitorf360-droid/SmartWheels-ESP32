// main.cpp
// Arquivo principal - Integra todos os módulos do SmartWheels

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "logger.h"
#include "storage_manager.h"
#include "gps_handler.h"
#include "button_handler.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "web_server.h"

// Variáveis de diagnóstico
int contadorReinicios = 0;
int contadorFalhasMQTT = 0;
static int ultimoErro = 0;
static unsigned long tempoUltimoErro = 0;

// ========== VARIÁVEIS DE CONTROLE ==========
static unsigned long ultimaTelemetria = 0;
static unsigned long ultimoHeartbeat = 0;
static unsigned long ultimaVerificacaoWiFi = 0;
static bool primeiraExecucao = true;

// ========== DECLARAÇÃO DE FUNÇÕES LOCAIS ==========
void processarEventos();
void enviarDadosPeriodicos();

// ========== SETUP ==========
void setup() {
    // Inicializa serial para debug
    Serial.begin(115200);
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
    delay(1000);

     
    
    Serial.println("\n\n========================================");
    Serial.println("   SMARTWHEELS - SISTEMA DE ALERTA");
    Serial.println("   Firmware " + String(VERSAO_FIRMWARE));
    Serial.println("========================================\n");
    
    // 1. Inicializa armazenamento (EEPROM)
    iniciarStorage();
    
    // 2. Carrega configurações salvas
    carregarConfiguracoes();

    // Carrega o estado do pânico salvo na EEPROM
    alertaPanicoAtivo = carregarEstadoPanico();
    if (alertaPanicoAtivo) {
    LOG_INFO("⚠️ Alerta de pânico estava ATIVO antes da reinicialização!");
    }
    
    // Carrega os estados salvos na EEPROM
    alertaPanicoAtivo = carregarEstadoPanico();
    alertaAcessibilidadeAtivo = carregarEstadoAcessibilidade();

if (alertaPanicoAtivo) {
    LOG_INFO("⚠️ Alerta de pânico estava ATIVO antes da reinicialização!");
}
if (alertaAcessibilidadeAtivo) {
    LOG_INFO("⚠️ Alerta de acessibilidade estava ATIVO antes da reinicialização!");
}

    // Carrega contadores de diagnóstico
    contadorReinicios = carregarContadorReinicios();
    contadorFalhasMQTT = carregarContadorFalhas();

    // Incrementa e salva o contador de reinícios (esta reinicialização)
    incrementarContadorReinicios();
    contadorReinicios = carregarContadorReinicios();

    LOG_INFO("📊 Diagnóstico - Reinícios: " + String(contadorReinicios) + " | Falhas: " + String(contadorFalhasMQTT));

    // 3. Inicializa sistema de logs
    setLogLevel(LOG_LEVEL_INFO);
    LOG_INFO("Sistema de logs inicializado");
    
    // 4. Inicializa GPS
    inicializarGPS();
    
    // 5. Inicializa botões
    inicializarBotoes();
    
    // 6. Inicializa WiFi
    inicializarWiFi();
    
    // 7. Tenta conectar ao WiFi salvo
    if (wifi_ssid != "") {
        LOG_INFO("Tentando conectar ao WiFi salvo...");
        if (conectarWiFiSalvo()) {
            LOG_INFO("WiFi conectado com sucesso!");
            
            // 8. Inicializa MQTT (só se tiver WiFi)
            inicializarMQTT();
            conectarMQTT();
            
            // 8.1 Limpa a fila de mensagens antigas
            while (!filaMensagens.empty()) {
                filaMensagens.pop();
            }
            LOG_INFO("Fila de mensagens limpa");
            
        } else {
            LOG_ERROR("Falha ao conectar WiFi - iniciando modo AP");
            iniciarModoAP();
        }
    } else {
        LOG_INFO("Nenhuma credencial WiFi salva - iniciando modo AP");
        iniciarModoAP();
    }
    
    // 9. Inicializa servidor web (sempre, tanto em modo STA quanto AP)
    inicializarWebServer();
    
    LOG_INFO("========================================");
    LOG_INFO("Sistema inicializado com sucesso!");
    LOG_INFO("Acesse: http://" + (emModoAP ? WiFi.softAPIP().toString() : WiFi.localIP().toString()));
    LOG_INFO("========================================\n");
    
    primeiraExecucao = false;
}

// ========== LOOP PRINCIPAL ==========
void loop() {
    // 1. Alimenta o watchdog (evita travamentos)
    esp_task_wdt_reset();
    
    // 2. Processa servidor web (sempre ativo)
    server.handleClient();
    
    // 3. Atualiza dados GPS continuamente
    atualizarGPS();
    
    // 4. Processa botões (detecta pressões longas)
    processarBotoes();
    
    // 5. Verifica eventos acionados pelos botões
    processarEventos();
    
    // 6. Gerencia WiFi (reconexão automática)
    verificarConexaoWiFi();
    
    // 7. Se WiFi estiver conectado, gerencia MQTT
    if (WiFi.status() == WL_CONNECTED) {
        loopMQTT();
        processarFilaMensagens();
    }
    
    // 8. Envia dados periódicos
    enviarDadosPeriodicos();
    
    // Pequeno delay para não sobrecarregar o processador
    delay(10);
}

// ========== FUNÇÃO PARA PROCESSAR EVENTOS DOS BOTÕES ==========
void processarEventos() {
    TipoEvento evento = verificarEvento();
    
    switch (evento) {
        case EVENTO_PANICO:
            LOG_INFO("🚨 Evento de PÂNICO detectado!");
            {
                Localizacao loc = obterLocalizacao(TIMEOUT_GPS_MS);
                enviarAlertaPanico(loc);
            }
            break;
            
        case EVENTO_ACESSIBILIDADE:
            LOG_INFO("⚠️ Evento de ACESSIBILIDADE detectado!");
            {
                Localizacao loc = obterLocalizacao(TIMEOUT_GPS_MS);
                enviarAlertaAcessibilidade(loc);
            }
            break;
            
        case EVENTO_NENHUM:
        default:
            // Nada a fazer
            break;
    }
}

// ========== FUNÇÃO PARA ENVIAR DADOS PERIÓDICOS ==========
void enviarDadosPeriodicos() {
    unsigned long agora = millis();
    
    // Telemetria a cada INTERVALO_TELEMETRIA (5 segundos)
    if (agora - ultimaTelemetria >= INTERVALO_TELEMETRIA) {
        ultimaTelemetria = agora;
        
        if (WiFi.status() == WL_CONNECTED && client.connected()) {
            enviarTelemetria();
        }
    }
    
    // Heartbeat a cada 30 segundos (para verificar se dispositivo está vivo)
    if (agora - ultimoHeartbeat >= 30000) {
        ultimoHeartbeat = agora;
        
        if (WiFi.status() == WL_CONNECTED && client.connected()) {
            enviarHeartbeat();
        }
        
        // Log periódico de status (apenas em DEBUG)
        if (logLevel == LOG_LEVEL_DEBUG) {
            LOG_DEBUG("Status: WiFi=" + String(WiFi.status() == WL_CONNECTED ? "OK" : "FALHA") +
                      " | MQTT=" + String(client.connected() ? "OK" : "FALHA") +
                      " | GPS=" + String(gpsTemSinal() ? "SIM" : "NAO") +
                      " | Sat=" + String(getNumeroSatelites()));
        }
    }
    
    // Verificação de conexão WiFi periódica (já é feita no verificarConexaoWiFi)
    // mas garantimos que não fique presa
    if (agora - ultimaVerificacaoWiFi >= 10000) {
        ultimaVerificacaoWiFi = agora;
        verificarConexaoWiFi();
    }
}