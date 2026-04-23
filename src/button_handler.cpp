// button_handler.cpp
// Implementação do gerenciamento dos botões

#include "button_handler.h"
#include "logger.h"
#include "storage_manager.h"

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
bool alertaPanicoAtivo = false;
bool alertaAcessibilidadeAtivo = false;

// Estados internos dos botões
static EstadoBotao botaoPanico = {
    BOTAO_PANICO_PIN,   // gpio
    false,              // pressionado
    false,              // alertaAtivo
    0,                  // inicioPressao
    EVENTO_PANICO       // tipo
};

static EstadoBotao botaoAcessibilidade = {
    BOTAO_AMARELO_PIN,  // gpio
    false,              // pressionado
    false,              // alertaAtivo
    0,                  // inicioPressao
    EVENTO_ACESSIBILIDADE // tipo
};

// ========== FUNÇÃO AUXILIAR INTERNA ==========
static void processarBotao(EstadoBotao &botao) {
    bool estadoAtual = digitalRead(botao.gpio) == LOW; // LOW porque tem pull-up
    
    // Botão começou a ser pressionado
    if (estadoAtual && !botao.pressionado) {
        botao.pressionado = true;
        botao.inicioPressao = millis();
        
        if (botao.tipo == EVENTO_PANICO) {
            LOG_INFO("Botão de PÂNICO pressionado - medindo tempo...");
        } else if (botao.tipo == EVENTO_ACESSIBILIDADE) {
            LOG_INFO("Botão de ACESSIBILIDADE pressionado - medindo tempo...");
        }
    }
    
    // Botão foi solto
    if (!estadoAtual && botao.pressionado) {
        botao.pressionado = false;
        unsigned long tempoPressionado = (millis() - botao.inicioPressao) / 1000;
        
        LOG_INFO("Botão solto - tempo pressionado: " + String(tempoPressionado) + " segundos");
        
        // Verifica se foi pressão longa (>= TEMPO_PANICO_MS)
        if (tempoPressionado >= (TEMPO_PANICO_MS / 1000)) {
            // Alterna o estado do alerta
            if (botao.tipo == EVENTO_PANICO) {
                alertaPanicoAtivo = !alertaPanicoAtivo;
                LOG_INFO(alertaPanicoAtivo ? "🚨 ALERTA DE PÂNICO ATIVADO!" : "✅ Alerta de pânico desativado");
                
                // Salva o estado na EEPROM
                salvarEstadoPanico(alertaPanicoAtivo);
                
            } else if (botao.tipo == EVENTO_ACESSIBILIDADE) {
                alertaAcessibilidadeAtivo = !alertaAcessibilidadeAtivo;
                LOG_INFO(alertaAcessibilidadeAtivo ? "⚠️ ALERTA DE ACESSIBILIDADE ATIVADO!" : "✅ Alerta de acessibilidade desativado");
                
                // Salva o estado na EEPROM
                salvarEstadoAcessibilidade(alertaAcessibilidadeAtivo);
            }
        }
    }
}

// ========== IMPLEMENTAÇÃO DAS FUNÇÕES PÚBLICAS ==========

void inicializarBotoes() {
    pinMode(botaoPanico.gpio, INPUT_PULLUP);
    pinMode(botaoAcessibilidade.gpio, INPUT_PULLUP);
    
    LOG_INFO("Botões inicializados:");
    LOG_INFO("  - Pânico: GPIO " + String(botaoPanico.gpio));
    LOG_INFO("  - Acessibilidade: GPIO " + String(botaoAcessibilidade.gpio));
    LOG_INFO("  - Tempo para ativação: " + String(TEMPO_PANICO_MS / 1000) + " segundos");
}

void processarBotoes() {
    processarBotao(botaoPanico);
    processarBotao(botaoAcessibilidade);
}

bool botaoPanicoFoiAcionado() {
    bool acionado = alertaPanicoAtivo;
    if (acionado) {
        alertaPanicoAtivo = false;  // Reseta após leitura
        salvarEstadoPanico(false);   // Salva o reset na EEPROM
        LOG_DEBUG("Flag de pânico lida e resetada");
    }
    return acionado;
}

bool botaoAcessibilidadeFoiAcionado() {
    bool acionado = alertaAcessibilidadeAtivo;
    if (acionado) {
        alertaAcessibilidadeAtivo = false;  // Reseta após leitura
        salvarEstadoAcessibilidade(false);   // Salva o reset na EEPROM
        LOG_DEBUG("Flag de acessibilidade lida e resetada");
    }
    return acionado;
}

void resetarAlertas() {
    alertaPanicoAtivo = false;
    alertaAcessibilidadeAtivo = false;
    salvarEstadoPanico(false);
    salvarEstadoAcessibilidade(false);
    LOG_INFO("Todos os alertas foram resetados");
}

TipoEvento verificarEvento() {
    if (botaoPanicoFoiAcionado()) {
        return EVENTO_PANICO;
    }
    if (botaoAcessibilidadeFoiAcionado()) {
        return EVENTO_ACESSIBILIDADE;
    }
    return EVENTO_NENHUM;
}