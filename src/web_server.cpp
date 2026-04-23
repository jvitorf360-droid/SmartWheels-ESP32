// web_server.cpp
// Implementação do servidor web

#include "web_server.h"
#include "logger.h"
#include "mqtt_manager.h"
#include "button_handler.h"

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
WebServer server(80);

// ========== PÁGINA PRINCIPAL (CONFIGURAÇÃO) ==========
void handleRoot() {
    String ip = emModoAP ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
    
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>SmartWheels - Configuração</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; background: #f5f5f5; }";
    html += ".container { background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
    html += "h1 { color: #333; text-align: center; margin-bottom: 30px; }";
    html += "h2 { color: #555; font-size: 18px; margin-top: 20px; }";
    html += ".form-group { margin-bottom: 20px; }";
    html += "label { display: block; margin-bottom: 5px; font-weight: bold; color: #555; }";
    html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 5px; font-size: 16px; box-sizing: border-box; }";
    html += ".btn { background: #007bff; color: white; border: none; padding: 12px 30px; font-size: 16px; border-radius: 5px; cursor: pointer; width: 100%; margin-top: 10px; }";
    html += ".btn:hover { background: #0056b3; }";
    html += ".btn-danger { background: #dc3545; }";
    html += ".btn-danger:hover { background: #c82333; }";
    html += ".status { padding: 15px; border-radius: 5px; margin-bottom: 20px; text-align: center; }";
    html += ".info { background: #d1ecf1; color: #0c5460; border: 1px solid #bee5eb; }";
    html += ".success { background: #d4edda; color: #155724; border: 1px solid #c3e6cb; }";
    html += ".error { background: #f8d7da; color: #721c24; border: 1px solid #f5c6cb; }";
    html += ".section { background: #f8f9fa; padding: 20px; border-radius: 5px; margin-bottom: 20px; border-left: 4px solid #007bff; }";
    html += ".info-text { font-size: 14px; color: #666; margin-top: 5px; }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>⚙️ SmartWheels - Configuração</h1>";
    
    // Status do sistema
    html += "<div class='status ";
    if (WiFi.status() == WL_CONNECTED) {
        html += "success'>🟢 Conectado ao WiFi";
    } else if (emModoAP) {
        html += "info'>🔴 Modo AP - Configure o WiFi";
    } else {
        html += "error'>⚠️ WiFi Desconectado";
    }
    html += "<br><strong>IP: " + ip + "</strong>";
    html += "<br>MAC: " + getMacAddress();
    html += "</div>";
    
    // Formulário de configuração
    html += "<form action='/save' method='post'>";
    
    // Seção WiFi
    html += "<div class='section'><h3>📶 Configuração WiFi</h3>";
    html += "<div class='form-group'><label for='wifi_ssid'>SSID da Rede WiFi:</label>";
    html += "<input type='text' id='wifi_ssid' name='wifi_ssid' value='" + wifi_ssid + "' placeholder='Nome da sua rede WiFi'></div>";
    html += "<div class='form-group'><label for='wifi_password'>Senha do WiFi:</label>";
    html += "<input type='password' id='wifi_password' name='wifi_password' value='" + wifi_password + "' placeholder='Senha do WiFi'></div>";
    html += "<div class='info-text'>📡 Após salvar, o dispositivo tentará conectar a esta rede</div></div>";
    
    // Seção ThingsBoard
    html += "<div class='section'><h3>🌐 Configuração ThingsBoard</h3>";
    html += "<div class='form-group'><label for='mqtt_server'>Servidor MQTT:</label>";
    html += "<input type='text' id='mqtt_server' name='mqtt_server' value='" + mqtt_server + "' placeholder='iotnode01.memind.com.br'></div>";
    html += "<div class='form-group'><label for='mqtt_port'>Porta MQTT:</label>";
    html += "<input type='text' id='mqtt_port' name='mqtt_port' value='" + mqtt_port + "' placeholder='1883'></div>";
    html += "<div class='form-group'><label for='mqtt_token'>Token do Dispositivo:</label>";
    html += "<input type='text' id='mqtt_token' name='mqtt_token' value='" + mqtt_token + "' placeholder='Token do ThingsBoard'></div>";
    html += "<div class='info-text'>🔑 O token é fornecido pelo ThingsBoard ao criar o dispositivo</div></div>";
    
    // Botões
    html += "<button type='submit' class='btn'>💾 Salvar Configurações</button>";
    html += "</form>";
    
    // Botão para limpar configurações
    html += "<form action='/clear' method='post' style='margin-top: 10px;'>";
    html += "<button type='submit' class='btn btn-danger' onclick='return confirm(\"Tem certeza? Todas as configurações serão apagadas.\")'>🗑️ Limpar Configurações</button>";
    html += "</form>";
    
    // Link para status
    html += "<div style='text-align: center; margin-top: 20px;'>";
    html += "<a href='/status' style='color: #007bff;'>📊 Ver Status Detalhado</a>";
    html += "</div>";
    
    html += "</div></body></html>";
    
    server.send(200, "text/html", html);
}

// ========== SALVAR CONFIGURAÇÕES ==========
void handleSave() {
    wifi_ssid = server.arg("wifi_ssid");
    wifi_password = server.arg("wifi_password");
    mqtt_server = server.arg("mqtt_server");
    mqtt_port = server.arg("mqtt_port");
    mqtt_token = server.arg("mqtt_token");
    
    salvarConfiguracoes();
    
    String html = "<html><head><meta http-equiv='refresh' content='3;url=/'>";
    html += "<style>body { font-family: Arial; text-align: center; padding: 50px; }";
    html += ".success { background: #d4edda; color: #155724; padding: 20px; border-radius: 5px; }</style></head>";
    html += "<body><div class='success'><h2>✅ Configurações Salvas!</h2>";
    html += "<p>Reiniciando o sistema em 3 segundos...</p>";
    html += "<p>O dispositivo tentará conectar à rede <strong>" + wifi_ssid + "</strong></p></div></body></html>";
    
    server.send(200, "text/html", html);
    delay(2000);
    ESP.restart();
}

// ========== LIMPAR CONFIGURAÇÕES ==========
void handleClear() {
    limparConfiguracoes();
    
    String html = "<html><head><meta http-equiv='refresh' content='3;url=/'>";
    html += "<style>body { font-family: Arial; text-align: center; padding: 50px; }";
    html += ".warning { background: #fff3cd; color: #856404; padding: 20px; border-radius: 5px; }</style></head>";
    html += "<body><div class='warning'><h2>🗑️ Configurações Apagadas!</h2>";
    html += "<p>Reiniciando o sistema em 3 segundos...</p>";
    html += "<p>O dispositivo entrará em modo AP para nova configuração</p></div></body></html>";
    
    server.send(200, "text/html", html);
    delay(2000);
    ESP.restart();
}

// ========== STATUS DO SISTEMA (TEXTO PLANO) ==========
void handleStatus() {
    String status = "========================================\n";
    status += "      SMARTWHEELS - STATUS DO SISTEMA      \n";
    status += "========================================\n\n";
    
    // Informações do dispositivo
    status += "📟 DISPOSITIVO\n";
    status += "  MAC Address: " + getMacAddress() + "\n";
    status += "  Panico (GPIO 15): " + String(digitalRead(15) == LOW ? "PRESSIONADO" : "SOLTO") + "\n";
status += "  Acessibilidade (GPIO 13): " + String(digitalRead(13) == LOW ? "PRESSIONADO" : "SOLTO") + "\n";
    status += "  Uptime: " + String(millis() / 1000) + " segundos\n";
    status += "  Memória livre: " + String(ESP.getFreeHeap()) + " bytes\n\n";
    
    // Status WiFi
    status += "📶 WIFI\n";
    if (WiFi.status() == WL_CONNECTED) {
        status += "  Status: CONECTADO\n";
        status += "  SSID: " + wifi_ssid + "\n";
        status += "  IP: " + WiFi.localIP().toString() + "\n";
        status += "  RSSI: " + String(WiFi.RSSI()) + " dBm\n";
    } else if (emModoAP) {
        status += "  Status: MODO AP (configuração)\n";
        status += "  SSID: " + String(WIFI_AP_SSID) + "\n";
        status += "  IP: " + WiFi.softAPIP().toString() + "\n";
    } else {
        status += "  Status: DESCONECTADO\n";
    }
    status += "\n";
    
    // Status MQTT
    status += "🌐 MQTT / THINGSBOARD\n";
    status += "  Servidor: " + mqtt_server + ":" + mqtt_port + "\n";
    status += "  Alerta panico ativo: " + String(alertaPanicoAtivo ? "SIM" : "NÃO") + "\n\n";
    status += "  Conexão: " + String(client.connected() ? "CONECTADO" : "DESCONECTADO") + "\n";
    status += "  Fila mensagens: " + String(filaMensagens.size()) + " pendentes\n\n";
    
    // Status GPS
    status += "🛰️ GPS\n";
    status += "  Sinal: " + String(gpsTemSinal() ? "SIM" : "NÃO") + "\n";
    status += "  Satélites: " + String(getNumeroSatelites()) + "\n";
    if (gps.location.isValid()) {
        status += "  Latitude: " + String(gps.location.lat(), 6) + "\n";
        status += "  Longitude: " + String(gps.location.lng(), 6) + "\n";
    }
    status += "\n";
    
    // Status dos botões
    status += "🚨 BOTÕES\n";
    status += "  Pânico (GPIO " + String(BOTAO_PANICO_PIN) + "): " + String(digitalRead(BOTAO_PANICO_PIN) == LOW ? "PRESSIONADO" : "SOLTO") + "\n";
    status += "  Acessibilidade (GPIO " + String(BOTAO_AMARELO_PIN) + "): " + String(digitalRead(BOTAO_AMARELO_PIN) == LOW ? "PRESSIONADO" : "SOLTO") + "\n";
    status += "  Alerta pânico ativo: " + String(alertaPanicoAtivo ? "SIM" : "NÃO") + "\n\n";
    
    status += "========================================\n";
    
    server.send(200, "text/plain", status);
}

// ========== PÁGINA NÃO ENCONTRADA ==========
void handleNotFound() {
    String html = "<html><head><meta http-equiv='refresh' content='3;url=/'>";
    html += "<style>body { font-family: Arial; text-align: center; padding: 50px; }";
    html += ".error { background: #f8d7da; color: #721c24; padding: 20px; border-radius: 5px; }</style></head>";
    html += "<body><div class='error'><h2>⚠️ Página não encontrada!</h2>";
    html += "<p>Redirecionando para a página inicial...</p></div></body></html>";
    
    server.send(404, "text/html", html);
}

// ========== INICIALIZAR SERVIDOR WEB ==========
void inicializarWebServer() {
    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.on("/clear", HTTP_POST, handleClear);
    server.on("/status", handleStatus);
    server.onNotFound(handleNotFound);
    
    server.begin();
    LOG_INFO("Web Server iniciado");
    LOG_INFO("  Página de configuração: http://" + (emModoAP ? WiFi.softAPIP().toString() : WiFi.localIP().toString()));
    LOG_INFO("  Status: http://" + (emModoAP ? WiFi.softAPIP().toString() : WiFi.localIP().toString()) + "/status");
}