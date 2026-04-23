// gps_handler.h
// Gerenciamento do módulo GPS

#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <Arduino.h>
#include <TinyGPS++.h>
#include "config.h"

// ========== ESTRUTURA PARA DADOS DE LOCALIZAÇÃO ==========
struct Localizacao {
    float latitude;
    float longitude;
    bool valido;
    int satelites;
    float precisao;  // em metros
    String fonte;    // "GPS" ou "SIMULADO"
};

// ========== VARIÁVEIS GLOBAIS ==========
extern TinyGPSPlus gps;
extern HardwareSerial GPS_SERIAL;

// ========== FUNÇÕES PÚBLICAS ==========
void inicializarGPS();
void processarGPS();
void atualizarGPS();

// Função principal para obter localização (com timeout)
Localizacao obterLocalizacao(int timeoutMs = TIMEOUT_GPS_MS);

// Funções auxiliares
bool gpsTemSinal();
int getNumeroSatelites();
void atualizarSimulacao();

#endif