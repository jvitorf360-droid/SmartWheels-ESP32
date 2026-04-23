# Tabela de Códigos de Erro - SmartWheels ESP32

## Códigos Gerais (100-199)

| Código | Descrição |
|--------|-----------|
| 100 | Sistema inicializado |
| 101 | Watchdog reset (sistema reiniciou após travamento) |

## WiFi (200-299)

| Código | Descrição |
|--------|-----------|
| 201 | Falha na conexão WiFi |
| 202 | WiFi desconectado |
| 203 | Timeout na conexão WiFi |

## MQTT / ThingsBoard (300-399)

| Código | Descrição |
|--------|-----------|
| 301 | Token não configurado |
| 302 | Falha na conexão MQTT |
| 303 | Falha ao publicar mensagem |

## GPS (400-499)

| Código | Descrição |
|--------|-----------|
| 401 | GPS sem sinal (usando simulação) |
| 402 | GPS retornou valor inválido |
| 403 | Timeout na leitura do GPS |

## Botões (500-599)

| Código | Descrição |
|--------|-----------|
| 501 | Botão pressionado |
| 502 | Pressão curta ignorada (menos de 3 segundos) |
| 503 | Botão amarelo não conectado |

## Armazenamento (600-699)

| Código | Descrição |
|--------|-----------|
| 601 | Falha na EEPROM |
| 602 | Configuração não salva |