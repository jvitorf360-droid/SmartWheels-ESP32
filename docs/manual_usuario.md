# Manual do Usuário – SmartWheels

## Dispositivo de alerta para cadeira de rodas

---

### 1. O que é o SmartWheels?

O SmartWheels é um dispositivo que você acopla na cadeira de rodas. Ele permite enviar alertas de emergência (pânico) ou registrar problemas de acessibilidade (como calçadas irregulares ou falta de rampas). O alerta é enviado para um responsável via WhatsApp, com a sua localização em tempo real.

---

### 2. O que vem na caixa

- 1 módulo principal (ESP32 com GPS e botões)
- 1 cabo USB para recarga
- 1 power bank (bateria portátil) – se aplicável
- 1 manual rápido (este documento)

---

### 3. Primeira configuração (Wi-Fi e token)

Antes de usar pela primeira vez, você precisa conectar o dispositivo à sua rede Wi-Fi e informar um token (fornecido pelo administrador do sistema).

**Passo a passo:**

| Etapa | O que fazer |
|-------|-------------|
| 1 | Conecte a power bank ao dispositivo. Ele ligará automaticamente. |
| 2 | No seu celular, acesse as configurações de Wi-Fi. |
| 3 | Conecte-se à rede `SmartWheels_Config`. |
| 4 | Digite a senha: `config123` |
| 5 | Abra o navegador (Chrome, Safari, Edge) e digite: `http://192.168.4.1` |
| 6 | Preencha os campos da página: <br> - **SSID:** nome da sua rede Wi-Fi <br> - **Senha:** sua senha <br> - **Servidor MQTT:** `iotnode01.memind.com.br` <br> - **Porta:** `1883` <br> - **Token:** (informado pelo administrador) |
| 7 | Clique em **"Salvar Configurações"**. |
| 8 | O dispositivo vai reiniciar e se conectar automaticamente à sua rede Wi-Fi. |

✅ **Pronto! O dispositivo está configurado e pronto para uso.**

---

### 4. Como usar os botões

O dispositivo tem dois botões: **VERMELHO** (pânico) e **AMARELO** (acessibilidade).

#### 🔴 Botão vermelho – Pânico (emergência)

| Ação | Resultado |
|------|-----------|
| Pressione por **3 segundos** | Alerta de pânico ativado. Um WhatsApp é enviado para o responsável com sua localização. |
| Pressione **novamente** por 3 segundos | Alerta de pânico desativado. |

> ⚠️ O botão só ativa após 3 segundos. Isso evita acionamentos acidentais.

#### 🟡 Botão amarelo – Acessibilidade (problemas na rua)

| Ação | Resultado |
|------|-----------|
| Pressione por **3 segundos** | Um alerta de acessibilidade é enviado ao responsável, informando o local do problema. O alerta é enviado uma única vez e se desativa automaticamente. |

> ⚠️ O botão só ativa após 3 segundos. Isso evita acionamentos acidentais.

---

### 5. Reset de fábrica (limpar configurações)

Se precisar apagar todas as configurações salvas (Wi-Fi, token, etc.) e começar de novo:

1. Pressione **os dois botões ao mesmo tempo** (vermelho e amarelo)
2. Mantenha pressionados por **10 segundos**
3. O dispositivo irá limpar todas as configurações e reiniciar automaticamente
4. Após reiniciar, ele voltará ao modo de configuração (`SmartWheels_Config`)

> ⚠️ **Atenção:** essa ação apaga permanentemente as configurações salvas. Use apenas quando necessário.

---

### 6. Como saber que o alerta foi enviado?

- **No dispositivo:** o LED interno pisca (se disponível).
- **No WhatsApp:** o responsável cadastrado recebe uma mensagem com sua localização e o tipo do alerta.
- **Na página de status** (opcional): acesse `http://[IP_DO_DISPOSITIVO]/status` para ver o diagnóstico completo.

---

### 7. Cuidados e manutenção

| Cuidado | Recomendação |
|---------|---------------|
| **Bateria** | Mantenha a power bank carregada. Uma carga dura cerca de 8 horas de uso contínuo. |
| **Água e calor** | Evite exposição à chuva ou calor extremo. |
| **Teste periódico** | Teste os botões uma vez por semana para garantir o funcionamento. |
| **Transporte** | Guarde o dispositivo em local seco e seguro quando não estiver usando. |

---

### 8. Problemas comuns e soluções

| Problema | O que fazer |
|----------|-------------|
| Não aparece a rede `SmartWheels_Config` | Desligue e ligue o dispositivo. Aguarde 30 segundos. |
| A página `192.168.4.1` não abre | Verifique se você está conectado na rede `SmartWheels_Config`. |
| Não conecta no Wi-Fi após configurar | Verifique se o SSID e a senha estão corretos. Reconfigure. |
| Botão não responde | Pressione por mais tempo (3 segundos). Se não funcionar, reinicie o dispositivo. |
| O WhatsApp não chega | Verifique com o administrador se o token está correto e se o responsável está cadastrado. |
| Dispositivo com muitas redes salvas | Pressione os dois botões por 10 segundos para resetar. |

---

### 9. Suporte

Em caso de problemas ou dúvidas, entre em contato com a equipe responsável pelo projeto SmartWheels.

---

**Manual do usuário – Versão 1.1**

Última atualização: outubro de 2026