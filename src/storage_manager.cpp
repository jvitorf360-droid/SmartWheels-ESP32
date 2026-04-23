// storage_manager.cpp
// Implementação do gerenciamento de armazenamento

#include "storage_manager.h"
#include "logger.h"

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
String wifi_ssid = "";
String wifi_password = "";
String mqtt_server = "";
String mqtt_port = "";
String mqtt_token = "";

// ========== IMPLEMENTAÇÃO ==========

void iniciarStorage() {
    EEPROM.begin(EEPROM_SIZE);
    LOG_INFO("EEPROM inicializada com " + String(EEPROM_SIZE) + " bytes");
}

void carregarConfiguracoes() {
    wifi_ssid = lerEEPROM(EEPROM_WIFI_SSID, 32);
    wifi_password = lerEEPROM(EEPROM_WIFI_PASS, 64);
    mqtt_server = lerEEPROM(EEPROM_MQTT_SERVER, 64);
    mqtt_port = lerEEPROM(EEPROM_MQTT_PORT, 6);
    mqtt_token = lerEEPROM(EEPROM_MQTT_TOKEN, 64);
    
    // Valores padrão se estiverem vazios
    if (mqtt_server == "") mqtt_server = MQTT_SERVER_PADRAO;
    if (mqtt_port == "") mqtt_port = MQTT_PORTA_PADRAO;
    
    LOG_INFO("Configurações carregadas da EEPROM");
    LOG_INFO("  WiFi SSID: " + wifi_ssid);
    LOG_INFO("  MQTT Server: " + mqtt_server);
    LOG_INFO("  MQTT Port: " + mqtt_port);
    
    if (mqtt_token != "") {
        LOG_INFO("  MQTT Token: CONFIGURADO");
    } else {
        LOG_INFO("  MQTT Token: NÃO CONFIGURADO");
    }
}

void salvarConfiguracoes() {
    escreverEEPROM(EEPROM_WIFI_SSID, wifi_ssid);
    escreverEEPROM(EEPROM_WIFI_PASS, wifi_password);
    escreverEEPROM(EEPROM_MQTT_SERVER, mqtt_server);
    escreverEEPROM(EEPROM_MQTT_PORT, mqtt_port);
    escreverEEPROM(EEPROM_MQTT_TOKEN, mqtt_token);
    
    EEPROM.commit();
    LOG_INFO("Configurações salvas na EEPROM");
}

void limparConfiguracoes() {
    // Limpa todas as configurações
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    
    wifi_ssid = "";
    wifi_password = "";
    mqtt_server = MQTT_SERVER_PADRAO;
    mqtt_port = MQTT_PORTA_PADRAO;
    mqtt_token = "";
    
    LOG_INFO("Todas as configurações foram apagadas");
}

String lerEEPROM(int start, int len) {
    String result = "";
    for (int i = start; i < start + len; i++) {
        byte c = EEPROM.read(i);
        if (c != 0 && c != 255) {
            result += (char)c;
        }
    }
    return result;
}

void escreverEEPROM(int start, String data) {
    for (int i = 0; i < data.length(); i++) {
        EEPROM.write(start + i, data[i]);
    }
    // Preenche o resto com zeros
    for (int i = data.length(); i < 64; i++) {
        EEPROM.write(start + i, 0);
    }
}

// ========== FUNÇÕES PARA PERSISTÊNCIA DE ESTADO ==========

void salvarEstadoPanico(bool ativo) {
    EEPROM.write(EEPROM_ESTADO_PANICO, ativo ? 1 : 0);
    EEPROM.commit();
    LOG_DEBUG("Estado de pânico salvo: " + String(ativo ? "ATIVO" : "INATIVO"));
}

bool carregarEstadoPanico() {
    byte valor = EEPROM.read(EEPROM_ESTADO_PANICO);
    bool ativo = (valor == 1);
    LOG_DEBUG("Estado de pânico carregado: " + String(ativo ? "ATIVO" : "INATIVO"));
    return ativo;
}

void salvarEstadoAcessibilidade(bool ativo) {
    EEPROM.write(EEPROM_ESTADO_ACESSIBILIDADE, ativo ? 1 : 0);
    EEPROM.commit();
    LOG_DEBUG("Estado de acessibilidade salvo: " + String(ativo ? "ATIVO" : "INATIVO"));
}

bool carregarEstadoAcessibilidade() {
    byte valor = EEPROM.read(EEPROM_ESTADO_ACESSIBILIDADE);
    bool ativo = (valor == 1);
    LOG_DEBUG("Estado de acessibilidade carregado: " + String(ativo ? "ATIVO" : "INATIVO"));
    return ativo;
}

// ========== FUNÇÕES PARA DIAGNÓSTICO ==========

void salvarContadorReinicios(int valor) {
    EEPROM.write(EEPROM_CONTADOR_REINICIOS, valor);
    EEPROM.commit();
    LOG_DEBUG("Contador de reinícios salvo: " + String(valor));
}

int carregarContadorReinicios() {
    int valor = EEPROM.read(EEPROM_CONTADOR_REINICIOS);
    if (valor < 0 || valor > 255) valor = 0;
    LOG_DEBUG("Contador de reinícios carregado: " + String(valor));
    return valor;
}

void salvarContadorFalhas(int valor) {
    EEPROM.write(EEPROM_CONTADOR_FALHAS, valor);
    EEPROM.commit();
    LOG_DEBUG("Contador de falhas salvo: " + String(valor));
}

int carregarContadorFalhas() {
    int valor = EEPROM.read(EEPROM_CONTADOR_FALHAS);
    if (valor < 0 || valor > 255) valor = 0;
    LOG_DEBUG("Contador de falhas carregado: " + String(valor));
    return valor;
}

void incrementarContadorReinicios() {
    int valor = carregarContadorReinicios();
    valor++;
    salvarContadorReinicios(valor);
}

void incrementarContadorFalhas() {
    int valor = carregarContadorFalhas();
    valor++;
    salvarContadorFalhas(valor);
}