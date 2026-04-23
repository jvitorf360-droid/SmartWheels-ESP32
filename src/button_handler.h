// button_handler.h
// Gerenciamento dos botões (pânico e acessibilidade)

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "config.h"

// ========== TIPOS DE EVENTO ==========
enum TipoEvento {
    EVENTO_NENHUM = 0,
    EVENTO_PANICO = 1,
    EVENTO_ACESSIBILIDADE = 2
};

// ========== ESTRUTURA PARA ESTADO DO BOTÃO ==========
struct EstadoBotao {
    int gpio;
    bool pressionado;
    bool alertaAtivo;
    unsigned long inicioPressao;
    TipoEvento tipo;
};

// ========== VARIÁVEIS GLOBAIS ==========
extern bool alertaPanicoAtivo;
extern bool alertaAcessibilidadeAtivo;

// ========== FUNÇÕES PÚBLICAS ==========
void inicializarBotoes();
void processarBotoes();
bool botaoPanicoFoiAcionado();
bool botaoAcessibilidadeFoiAcionado();
void resetarAlertas();
TipoEvento verificarEvento();

#endif