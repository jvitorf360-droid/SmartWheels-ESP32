// gps_handler.cpp
// Implementação do gerenciamento do GPS

#include "gps_handler.h"
#include "logger.h"

// ========== DEFINIÇÃO DAS VARIÁVEIS GLOBAIS ==========
TinyGPSPlus gps;
HardwareSerial GPS_SERIAL(2);

// Variáveis para simulação (fallback)
static float simLatitude = SIM_LATITUDE_PADRAO;
static float simLongitude = SIM_LONGITUDE_PADRAO;
static unsigned long ultimoMovimentoSimulado = 0;

// ========== IMPLEMENTAÇÃO ==========

void inicializarGPS() {
    GPS_SERIAL.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    LOG_INFO("GPS inicializado na Serial2 (RX=" + String(GPS_RX_PIN) + ", TX=" + String(GPS_TX_PIN) + ")");
    LOG_INFO("  Baud rate: " + String(GPS_BAUD));
}

void processarGPS() {
    // Lê todos os dados disponíveis da serial
    while (GPS_SERIAL.available() > 0) {
        char c = GPS_SERIAL.read();
        if (gps.encode(c)) {
            // Um frame NMEA completo foi processado
            // Dados atualizados, prontos para uso
        }
    }
}

void atualizarGPS() {
    // Atualiza os dados GPS (chamar frequentemente no loop)
    processarGPS();
    
    // Atualiza simulação para fallback (movimento aleatório lento)
    if (millis() - ultimoMovimentoSimulado > 30000) {  // A cada 30 segundos
        ultimoMovimentoSimulado = millis();
        // Pequena variação para simular movimento
        simLatitude += (random(-50, 50) / 10000000.0);
        simLongitude += (random(-50, 50) / 10000000.0);
    }
}

Localizacao obterLocalizacao(int timeoutMs) {
    Localizacao loc;
    
    // Inicializa com valores padrão de simulação (fallback)
    loc.latitude = simLatitude;
    loc.longitude = simLongitude;
    loc.valido = false;
    loc.satelites = 0;
    loc.precisao = 50.0;
    loc.fonte = "SIMULADO";
    
    LOG_INFO("Buscando sinal GPS por " + String(timeoutMs) + " ms...");
    
    unsigned long inicio = millis();
    
    while (millis() - inicio < timeoutMs) {
        // Processa dados GPS disponíveis
        while (GPS_SERIAL.available() > 0) {
            if (gps.encode(GPS_SERIAL.read())) {
                if (gps.location.isValid()) {
                    float lat = gps.location.lat();
                    float lng = gps.location.lng();
                    
                    // VALIDAÇÃO: verifica se os valores são válidos (não são NaN)
                    if (!isnan(lat) && !isnan(lng)) {
                        loc.latitude = lat;
                        loc.longitude = lng;
                        loc.satelites = gps.satellites.value();
                        loc.valido = true;
                        loc.fonte = "GPS";
                        
                        // Calcula precisão baseada no número de satélites
                        if (loc.satelites >= 8) {
                            loc.precisao = 3.0;   // Excelente
                        } else if (loc.satelites >= 5) {
                            loc.precisao = 5.0;   // Bom
                        } else if (loc.satelites >= 3) {
                            loc.precisao = 10.0;  // Regular
                        } else {
                            loc.precisao = 20.0;  // Ruim
                        }
                        
                        LOG_INFO("GPS REAL obtido: " + String(loc.latitude, 6) + ", " + String(loc.longitude, 6));
                        LOG_INFO("  Satélites: " + String(loc.satelites) + ", Precisão: " + String(loc.precisao) + "m");
                        
                        return loc;
                    } else {
                        LOG_ERROR("GPS retornou valores inválidos (NaN) - continuando busca");
                    }
                }
            }
        }
        delay(10);
    }
    
    // Se não conseguiu GPS válido, usa simulação com pequena variação
    LOG_INFO("GPS não disponível - usando simulação");
    
    // Atualiza simulação para dar sensação de movimento
    simLatitude += (random(-100, 100) / 10000000.0);
    simLongitude += (random(-100, 100) / 10000000.0);
    
    loc.latitude = simLatitude;
    loc.longitude = simLongitude;
    loc.valido = false;
    loc.satelites = 0;
    loc.precisao = 50.0;
    loc.fonte = "SIMULADO";
    
    LOG_INFO("Localização simulada: " + String(loc.latitude, 6) + ", " + String(loc.longitude, 6));
    
    return loc;
}

bool gpsTemSinal() {
    processarGPS();
    return gps.location.isValid();
}

int getNumeroSatelites() {
    processarGPS();
    if (gps.satellites.isValid()) {
        return gps.satellites.value();
    }
    return 0;
}

void atualizarSimulacao() {
    // Pequena variação aleatória para simular deslocamento
    simLatitude += (random(-100, 100) / 10000000.0);
    simLongitude += (random(-100, 100) / 10000000.0);
    
    // Limita dentro de SP (valores aproximados)
    simLatitude = constrain(simLatitude, -23.7, -23.4);
    simLongitude = constrain(simLongitude, -46.8, -46.4);
}