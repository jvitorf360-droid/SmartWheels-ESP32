# SmartWheels

## Sobre o Projeto

O **SmartWheels** é um sistema de alerta desenvolvido para cadeiras de rodas, com o objetivo de oferecer mais segurança e autonomia para pessoas com mobilidade reduzida. 

Através de um dispositivo ESP32 acoplado à cadeira, o usuário pode acionar um botão de pânico em situações de emergência. O dispositivo captura a localização em tempo real (via GPS) e envia os dados para a plataforma ThingsBoard, permitindo que um responsável seja notificado imediatamente (por exemplo, via WhatsApp, através de integrações futuras).

O projeto nasceu da necessidade de criar uma camada extra de proteção para pessoas que enfrentam desafios diários de deslocamento, oferecendo mais tranquilidade tanto para o usuário quanto para seus familiares ou cuidadores.

## Hardware Necessário

| Componente | Especificação | Pino / Porta |
|------------|--------------|--------------|
| ESP32 | DOIT DevKit V1 | — |
| Módulo GPS | Comunicação serial | RX = 16, TX = 17 |
| Botão de Pânico | Push button | GPIO 15 |
| Botão de Acessibilidade | Push button | GPIO 13 (implementação futura) |

## Bibliotecas Utilizadas

```ini
lib_deps =
    bblanchon/ArduinoJson @ ^7.0.0
    knolleary/PubSubClient @ ^2.8
    mikalhart/TinyGPSPlus @ ^1.0.3
Funcionalidades do Sistema
Conexão WiFi com fallback automático

Modo Access Point (AP) para configuração inicial

Leitura de GPS com simulação (fallback) em ambientes sem sinal

Botão de pânico com detecção de pressão longa (3 segundos)

Envio de alertas para o ThingsBoard via MQTT

Payloads compactos (otimizados para economia de bytes)

Persistência de estado na EEPROM (mantém o alerta ativo mesmo após reinicialização)

Telemetria de diagnóstico (reinicializações e falhas)

Interface web embarcada para configuração de WiFi e token

Reconexão automática WiFi e MQTT

Watchdog para reinicialização segura em caso de travamento

Como Configurar o Dispositivo
1. Primeira inicialização (modo AP)
Ao ligar o ESP32 pela primeira vez (ou sem configurações salvas), ele entra em modo Access Point.

Conecte-se à rede Wi-Fi: SmartWheels_Config

Senha: config123

Acesse no navegador: http://192.168.4.1

2. Página de configuração
Na interface web, informe os seguintes dados:

SSID da sua rede WiFi

Senha da sua rede WiFi

Servidor MQTT: iotnode01.memind.com.br

Porta MQTT: 1883

Token do dispositivo (gerado no ThingsBoard)

Após salvar, o dispositivo reinicia e tenta se conectar à rede informada.

3. Verificação de status
Acesse a rota /status do dispositivo (ex: http://192.168.0.100/status) para obter um diagnóstico completo.

Estrutura do Código
Arquivo	Função
main.cpp	Integração de todos os módulos
config.h	Constantes e pinagens
logger.h/cpp	Logs estruturados (DEBUG, INFO, ERROR)
storage_manager.h/cpp	Gerenciamento da EEPROM e persistência
gps_handler.h/cpp	Leitura do GPS com fallback para simulação
button_handler.h/cpp	Detecção dos botões (pânico e acessibilidade)
wifi_manager.h/cpp	Conexão WiFi e modo AP
mqtt_manager.h/cpp	MQTT, fila de mensagens e ThingsBoard
web_server.h/cpp	Interface web de configuração
Documentação Complementar
Mapeamento de Payloads MQTT — significado de cada campo JSON

Códigos de Erro — tabela de diagnóstico

Troubleshooting
Problema	Possível solução
Não conecta no WiFi	Acesse o modo AP e reconfigure as credenciais
MQTT não conecta	Verifique o token e o servidor MQTT
GPS sem sinal	O sistema entra automaticamente em modo de simulação
Página web não acessível	Confirme o IP correto no Serial Monitor
Mensagens MQTT falham	O buffer MQTT foi ajustado para 512 bytes (suporta payloads maiores)
Autor
João Vitor de Oliveira Francisco — Desenvolvimento do firmware ESP32

Licença
Projeto acadêmico — AACD / PIME