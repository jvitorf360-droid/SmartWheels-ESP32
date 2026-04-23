// wifi_manager.cpp
// Implementação do gerenciamento da conexão WiFi

#include "wifi_manager.h"
#include "logger.h"

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
bool emModoAP = false;

// Variáveis internas para controle de reconexão
static int tentativasReconexao = 0;
static unsigned long ultimaTentativaReconexao = 0;
static bool wifiConfigurado = false;

// ========== IMPLEMENTAÇÃO ==========

void inicializarWiFi() {
    LOG_INFO("Inicializando WiFi...");
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // Vamos gerenciar manualmente
    
    // Verifica se há credenciais salvas
    if (wifi_ssid != "") {
        LOG_INFO("Credenciais WiFi encontradas: " + wifi_ssid);
        wifiConfigurado = true;
    } else {
        LOG_INFO("Nenhuma credencial WiFi salva");
        wifiConfigurado = false;
    }
}

bool conectarWiFi(String ssid, String senha, int timeoutSegundos) {
    LOG_INFO("Conectando ao WiFi: " + ssid);
    
    WiFi.begin(ssid.c_str(), senha.c_str());
    
    int tentativas = 0;
    int maxTentativas = timeoutSegundos * 2;  // 2 tentativas por segundo
    
    while (WiFi.status() != WL_CONNECTED && tentativas < maxTentativas) {
        delay(500);
        Serial.print(".");
        tentativas++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO("✅ WiFi conectado com sucesso!");
        LOG_INFO("  IP: " + WiFi.localIP().toString());
        LOG_INFO("  RSSI: " + String(WiFi.RSSI()) + " dBm");
        
        emModoAP = false;
        tentativasReconexao = 0;
        wifiConfigurado = true;
        
        return true;
    } else {
        LOG_ERROR("❌ Falha ao conectar ao WiFi: " + ssid);
        return false;
    }
}

bool conectarWiFiSalvo() {
    if (wifi_ssid == "") {
        LOG_INFO("Nenhuma credencial WiFi salva para conectar");
        return false;
    }
    
    return conectarWiFi(wifi_ssid, wifi_password);
}

void iniciarModoAP() {
    LOG_INFO("Iniciando modo Access Point...");
    
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
    emModoAP = true;
    
    LOG_INFO("📡 Modo AP Ativo");
    LOG_INFO("  SSID: " + String(WIFI_AP_SSID));
    LOG_INFO("  Senha: " + String(WIFI_AP_PASSWORD));
    LOG_INFO("  IP: " + WiFi.softAPIP().toString());
}

void desconectarWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    LOG_INFO("WiFi desconectado e modo desligado");
}

void tentarReconexaoWiFi() {
    // Se já estiver conectado, não faz nada
    if (WiFi.status() == WL_CONNECTED) {
        tentativasReconexao = 0;
        return;
    }
    
    // Se não tem credenciais salvas, não tenta reconectar
    if (wifi_ssid == "") {
        return;
    }
    
    // Calcula delay com backoff exponencial: 1s, 2s, 4s, 8s, 16s, 32s, 64s (max)
    int delayMs = min(1000 * (1 << tentativasReconexao), 64000);
    
    LOG_INFO("🔄 Tentativa " + String(tentativasReconexao + 1) + " de reconexão WiFi");
    LOG_DEBUG("  Aguardando " + String(delayMs) + " ms antes de tentar");
    
    delay(delayMs);
    
    WiFi.reconnect();
    tentativasReconexao++;
    
    // Verifica se conseguiu
    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO("✅ WiFi reconectado com sucesso!");
        LOG_INFO("  IP: " + WiFi.localIP().toString());
        tentativasReconexao = 0;
        
        // Sai do modo AP se estava nele
        if (emModoAP) {
            emModoAP = false;
            LOG_INFO("Saindo do modo AP");
        }
    }
}

void verificarConexaoWiFi() {
    // Se não tem credenciais, não tenta
    if (wifi_ssid == "") {
        return;
    }
    
    // Se está conectado, resetar contador
    if (WiFi.status() == WL_CONNECTED) {
        tentativasReconexao = 0;
        return;
    }
    
    // Se perdeu conexão, tenta reconectar (no máximo a cada 5 segundos)
    unsigned long agora = millis();
    if (agora - ultimaTentativaReconexao > 5000) {
        ultimaTentativaReconexao = agora;
        tentarReconexaoWiFi();
    }
}

WiFiStatus obterStatusWiFi() {
    WiFiStatus status;
    status.conectado = (WiFi.status() == WL_CONNECTED);
    status.ssid = wifi_ssid;
    status.rssi = WiFi.RSSI();
    
    if (status.conectado) {
        status.ip = WiFi.localIP().toString();
    } else if (emModoAP) {
        status.ip = WiFi.softAPIP().toString();
    } else {
        status.ip = "0.0.0.0";
    }
    
    return status;
}

String getMacAddress() {
    return WiFi.macAddress();
}