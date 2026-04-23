// mqtt_manager.h
// Gerenciamento da comunicação MQTT com ThingsBoard

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "config.h"
#include "storage_manager.h"
#include "gps_handler.h"
#include <queue>

// ========== ESTRUTURA PARA MENSAGEM PENDENTE ==========
struct MensagemPendente {
    String topico;
    String payload;
    unsigned long timestamp;
};

// ========== VARIÁVEIS GLOBAIS ==========
extern WiFiClient espClient;
extern PubSubClient client;
extern std::queue<MensagemPendente> filaMensagens;

// ========== FUNÇÕES PÚBLICAS ==========
void inicializarMQTT();
bool conectarMQTT();
void desconectarMQTT();
void loopMQTT();
void publicarMensagem(String topico, String payload);
void publicarSeguro(String topico, String payload);
void processarFilaMensagens();
void enviarAlertaPanico(Localizacao loc);
void enviarAlertaAcessibilidade(Localizacao loc);
void enviarTelemetria();
void enviarHeartbeat();
bool mqttConectado();

#endif