// logger.h
// Sistema de logs estruturados com níveis

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

// ========== NÍVEIS DE LOG ==========
#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_ERROR   2
#define LOG_LEVEL_NONE    3

// ========== CONFIGURAÇÃO GLOBAL ==========
extern int logLevel;

// ========== FUNÇÕES PÚBLICAS ==========
void setLogLevel(int level);
int getLogLevel();

// ========== MACROS PARA USO NO CÓDIGO ==========
#define LOG_DEBUG(msg) if (logLevel <= LOG_LEVEL_DEBUG) { Serial.print("[DEBUG] "); Serial.println(msg); }
#define LOG_INFO(msg)  if (logLevel <= LOG_LEVEL_INFO)  { Serial.print("[INFO] ");  Serial.println(msg); }
#define LOG_ERROR(msg) if (logLevel <= LOG_LEVEL_ERROR) { Serial.print("[ERROR] "); Serial.println(msg); }

// Versões para mensagens com variáveis (String)
#define LOG_DEBUG_S(msg) if (logLevel <= LOG_LEVEL_DEBUG) { Serial.print("[DEBUG] "); Serial.println(msg); }
#define LOG_INFO_S(msg)  if (logLevel <= LOG_LEVEL_INFO)  { Serial.print("[INFO] ");  Serial.println(msg); }
#define LOG_ERROR_S(msg) if (logLevel <= LOG_LEVEL_ERROR) { Serial.print("[ERROR] "); Serial.println(msg); }

#endif