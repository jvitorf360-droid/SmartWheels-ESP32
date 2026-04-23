// logger.cpp
// Implementação do sistema de logs

#include "logger.h"

// ========== VARIÁVEL GLOBAL ==========
int logLevel = LOG_LEVEL_INFO;  // Começa com INFO, pode mudar para DEBUG

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES ==========
void setLogLevel(int level) {
    logLevel = level;
    
    switch(level) {
        case LOG_LEVEL_DEBUG:
            Serial.println("[LOG] Nível de log alterado para DEBUG");
            break;
        case LOG_LEVEL_INFO:
            Serial.println("[LOG] Nível de log alterado para INFO");
            break;
        case LOG_LEVEL_ERROR:
            Serial.println("[LOG] Nível de log alterado para ERROR");
            break;
        case LOG_LEVEL_NONE:
            Serial.println("[LOG] Nível de log alterado para NENHUM");
            break;
        default:
            Serial.println("[LOG] Nível de log desconhecido");
    }
}

int getLogLevel() {
    return logLevel;
}