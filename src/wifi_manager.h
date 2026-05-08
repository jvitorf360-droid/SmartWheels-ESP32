// wifi_manager.h
// Gerenciamento da conexão WiFi

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#define TINY_GSM_MODEM_SIM7600

#include <TinyGsmClient.h>
#include "config.h"
#include "storage_manager.h"

extern HardwareSerial SerialAT;
extern TinyGsm modem;

// ========== ESTRUTURA PARA STATUS DO WiFi ==========
struct WiFiStatus {
    bool conectado;
    String ssid;
    String ip;
    int rssi;
};

// ========== VARIÁVEIS GLOBAIS ==========
extern bool emModoAP;

// ========== FUNÇÕES PÚBLICAS ==========
void inicializarWiFi();
bool conectarWiFi(String ssid, String senha, int timeoutSegundos = 20);
bool conectarWiFiSalvo();
void iniciarModoAP();
void desconectarWiFi();
void tentarReconexaoWiFi();
void verificarConexaoWiFi();
WiFiStatus obterStatusWiFi();
String getMacAddress();

#endif