// web_server.h
// Servidor web para configuração e status

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "config.h"
#include "storage_manager.h"
#include "wifi_manager.h"
#include "gps_handler.h"
#include "mqtt_manager.h"

// ========== VARIÁVEIS GLOBAIS ==========
extern WebServer server;

// ========== FUNÇÕES PÚBLICAS ==========
void inicializarWebServer();
void handleRoot();
void handleSave();
void handleStatus();
void handleNotFound();

#endif