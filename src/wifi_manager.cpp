// wifi_manager.cpp
// Implementação do gerenciamento da conexão WiFi
const char apn[] = "zap.vivo.com.br";

#include "wifi_manager.h"
#include "logger.h"

HardwareSerial SerialAT(1);
TinyGsm modem(SerialAT);

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
bool emModoAP = false;

// Variáveis internas para controle de reconexão
static int tentativasReconexao = 0;
static unsigned long ultimaTentativaReconexao = 0;
static bool wifiConfigurado = false;

// ========== IMPLEMENTAÇÃO ==========

void inicializarWiFi() {
    LOG_INFO("Inicializando WiFi...");
    SerialAT.begin(115200, SERIAL_8N1, 16, 17);

    LOG_INFO("Inicializando modem LTE...");

    modem.restart();  // Vamos gerenciar manualmente
    
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

    LOG_INFO("Conectando LTE/4G...");

    const char apn[] = "zap.vivo.com.br";

    bool conectado = modem.gprsConnect(apn, "", "");

    if (conectado) {

        LOG_INFO("✅ LTE conectado!");

        emModoAP = false;
        tentativasReconexao = 0;

        return true;

    } else {

        LOG_ERROR("❌ Falha conexão LTE");
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
    
    //WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
    emModoAP = true;
    
    LOG_INFO("📡 Modo AP Ativo");
    LOG_INFO("  SSID: " + String(WIFI_AP_SSID));
    LOG_INFO("  Senha: " + String(WIFI_AP_PASSWORD));
    LOG_INFO("Modo AP desativado no LTE");
}

void desconectarWiFi() {
    //WiFi.disconnect(true);
    //WiFi.mode(WIFI_OFF);
    LOG_INFO("WiFi desconectado e modo desligado");
}

void tentarReconexaoWiFi() {
    // Se já estiver conectado, não faz nada
    if (modem.isGprsConnected()) {
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
    
    modem.gprsConnect(apn, "", "");
    tentativasReconexao++;
    
    // Verifica se conseguiu
    if (modem.isGprsConnected()) {
        LOG_INFO("✅ WiFi reconectado com sucesso!");
        LOG_INFO("  IP: " + modem.getLocalIP());
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
    if (modem.isGprsConnected()) {
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
    status.conectado = (modem.isGprsConnected());
    status.ssid = wifi_ssid;
    status.rssi = 0;
    
    if (status.conectado) {
        status.ip = modem.getLocalIP();
    } else if (emModoAP) {
        status.ip = modem.getLocalIP();
    } else {
        status.ip = "0.0.0.0";
    }
    
    return status;
}

String getMacAddress() {
    return modem.getModemInfo();
}