// config.h
// Constantes e definições do projeto SmartWheels
#include <esp_task_wdt.h>
#ifndef CONFIG_H
#define CONFIG_H

// ========== PINAGEM ==========
#define BOTAO_PANICO_PIN    15
#define BOTAO_AMARELO_PIN   18
#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
#define GPS_BAUD            9600

// ========== TEMPOS (em milissegundos) ==========
#define TEMPO_PANICO_MS      3000   // 3 segundos para ativar pânico
#define TIMEOUT_GPS_MS       5000   // 5 segundos aguardando GPS
#define WATCHDOG_TIMEOUT_S   10     // 10 segundos para watchdog
#define INTERVALO_TELEMETRIA 5000   // 5 segundos entre telemetrias

// ========== WIFI ==========
#define WIFI_AP_SSID         "SmartWheels_Config"
#define WIFI_AP_PASSWORD     "config123"
#define MAX_TENTATIVAS_WIFI  20

// ========== MQTT / THINGSBOARD ==========
#define MQTT_SERVER_PADRAO   "iotnode01.memind.com.br"
#define MQTT_PORTA_PADRAO    "1883"
#define TOPICO_TELEMETRIA    "v1/devices/me/telemetry"

// ========== FIRMWARE ==========
#define VERSAO_FIRMWARE      "1.3.0"
#define VERSAO_PAYLOAD       "1.0"

// ========== EEPROM ==========
#define EEPROM_SIZE          512
#define EEPROM_WIFI_SSID     0
#define EEPROM_WIFI_PASS     32
#define EEPROM_MQTT_SERVER   96
#define EEPROM_MQTT_PORT     160
#define EEPROM_MQTT_TOKEN    166
#define EEPROM_ESTADO_PANICO 230
#define EEPROM_ESTADO_ACESSIBILIDADE 231
#define EEPROM_CONTADOR_REINICIOS   232  
#define EEPROM_CONTADOR_FALHAS      233

// ========== SIMULAÇÃO (fallback) ==========
#define SIM_LATITUDE_PADRAO  -23.550520
#define SIM_LONGITUDE_PADRAO -46.633308

#endif