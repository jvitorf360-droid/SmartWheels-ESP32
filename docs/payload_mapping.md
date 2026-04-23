# Mapeamento de Payloads MQTT - SmartWheels ESP32

## Tópico MQTT
`v1/devices/me/telemetry`

## Tabela de Campos

| Campo | Tipo | Significado | Valores Possíveis |
|-------|------|-------------|-------------------|
| `e` | string | Tipo do evento | `PAN` (pânico), `ACC` (acessibilidade), `TEL` (telemetria), `HBT` (heartbeat) |
| `v` | string | Versão do payload | `1.0` |
| `ts` | number | Timestamp (millis do ESP32) | 0 a 4294967295 |
| `id` | string | MAC address do dispositivo | Ex: `40:91:51:2A:0D:98` |
| `fw` | string | Versão do firmware | `1.3.0` |
| `lat` | float | Latitude | Ex: `-23.550528` |
| `lng` | float | Longitude | Ex: `-46.633312` |
| `prec` | number | Precisão (metros) | `3` a `50` |
| `src` | string | Fonte da localização | `GPS`, `SIMULADO`, `FALLBACK` |
| `sat` | number | Número de satélites | `0` a `12` |
| `rssi` | number | Sinal WiFi (dBm) | Ex: `-67`, `44` |
| `bat` | number | Bateria (%) | `0` a `100` |
| `up` | number | Uptime (segundos) | millis() / 1000 |
| `gps` | number | GPS válido? | `1` (sim), `0` (não) |
| `reinicios` | number | Reinicializações | Contador salvo na EEPROM |
| `falhas` | number | Falhas MQTT | Contador salvo na EEPROM |

## Exemplos de Payloads

### Alerta de Pânico (`e = PAN`)
### Alerta de Acessibilidade (`e = ACC`)
### Telemetria com Diagnóstico (`e = TEL`)
### Heartbeat (`e = HBT`)
```json
{
  "e": "PAN",
  "v": "1.0",
  "ts": 28604,
  "id": "40:91:51:2A:0D:98",
  "lat": -23.550528,
  "lng": -46.633312,
  "prec": 50,
  "src": "SIMULADO",
  "sat": 0,
  "rssi": 44,
  "bat": 85
}
{
  "e": "ACC",
  "v": "1.0",
  "ts": 28604,
  "id": "40:91:51:2A:0D:98",
  "lat": -23.550528,
  "lng": -46.633312,
  "prec": 50,
  "src": "SIMULADO",
  "sat": 0,
  "rssi": 44,
  "bat": 85
}
{
  "e": "TEL",
  "ts": 28604,
  "rssi": 44,
  "gps": 0,
  "sat": 0,
  "up": 3600,
  "reinicios": 3,
  "falhas": 0
}
{
  "e": "HBT",
  "ts": 28604,
  "id": "40:91:51:2A:0D:98",
  "fw": "1.3.0"
}