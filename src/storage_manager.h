// storage_manager.h
// Gerenciamento de armazenamento na EEPROM

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

// ========== VARIÁVEIS GLOBAIS (configurações) ==========
extern String wifi_ssid;
extern String wifi_password;
extern String mqtt_server;
extern String mqtt_port;
extern String mqtt_token;

// ========== FUNÇÕES PÚBLICAS ==========
void iniciarStorage();
void carregarConfiguracoes();
void salvarConfiguracoes();
void limparConfiguracoes();

String lerEEPROM(int start, int len);
void escreverEEPROM(int start, String data);

void salvarEstadoPanico(bool ativo);
bool carregarEstadoPanico();

void salvarEstadoAcessibilidade(bool ativo); 
bool carregarEstadoAcessibilidade();

// Funções para diagnóstico
void salvarContadorReinicios(int valor);
int carregarContadorReinicios();
void salvarContadorFalhas(int valor);
int carregarContadorFalhas();
void incrementarContadorReinicios();
void incrementarContadorFalhas();

#endif