markdown
# Mapeamento de Payloads MQTT - SmartWheels ESP32

## Tópico MQTT
`v1/devices/me/telemetry`

## Tabela de Campos

| Campo | Tipo | Significado | Valores Possíveis |
|-------|------|-------------|-------------------|
| `botao_acionado` | string | Botão que foi acionado | `panico` ou `acessibilidade` |
| `estado_acionamento` | number | Estado do alerta | `1` (ativado) ou `0` (desativado) |
| `latitude` | float | Latitude da localização | Ex: `-23.550520` |
| `longitude` | float | Longitude da localização | Ex: `-46.633308` |
| `token` | string | Token de autenticação do dispositivo no ThingsBoard | Token gerado no ThingsBoard |

## Exemplos de Payloads

### Alerta de Pânico (`botao_acionado = panico`)

**Ativação:**
```json
{
  "botao_acionado": "panico",
  "estado_acionamento": 1,
  "latitude": -23.550520,
  "longitude": -46.633308,
  "token": "S2iLdye5coNPOJJQThKi"
}
Desativação:

json
{
  "botao_acionado": "panico",
  "estado_acionamento": 0,
  "latitude": -23.550520,
  "longitude": -46.633308,
  "token": "S2iLdye5coNPOJJQThKi"
}
Alerta de Acessibilidade (botao_acionado = acessibilidade)
Ativação (envio único):

json
{
  "botao_acionado": "acessibilidade",
  "estado_acionamento": 1,
  "latitude": -23.550520,
  "longitude": -46.633308,
  "token": "S2iLdye5coNPOJJQThKi"
}
Observação: o alerta de acessibilidade é enviado uma única vez e se desativa automaticamente. Não há payload de desativação para este botão.

Observações Importantes
O payload foi simplificado para o formato enxuto, contendo apenas os campos essenciais para o backend.

O campo token é o mesmo utilizado para autenticação no ThingsBoard.

A localização pode ser proveniente de GPS real ou de simulação (fallback), mas no payload o formato é idêntico.

O campo estado_acionamento indica se o alerta está sendo ativado (1) ou desativado (0).

Versionamento
Versão	Data	Alterações
1.0	2026	Versão inicial com formato compacto (siglas)
1.4	2026	Formato enxuto padronizado com campos descritivos