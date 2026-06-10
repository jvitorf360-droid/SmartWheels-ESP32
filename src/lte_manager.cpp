#define TINY_GSM_MODEM_SIM7600

#include <TinyGsmClient.h>
#include "config.h"
#include "lte_manager.h"
#include "wifi_manager.h"

bool inicializarLTE()
{
    SerialAT.begin(
        LTE_BAUD,
        SERIAL_8N1,
        LTE_RX_PIN,
        LTE_TX_PIN
    );

    delay(3000);

    return modem.restart();
}

bool conectarLTE()
{
    if (!modem.waitForNetwork())
        return false;

    return modem.gprsConnect(
        LTE_APN,
        LTE_USER,
        LTE_PASS
    );
}

bool lteConectado()
{
    return modem.isGprsConnected();
}

void verificarLTE()
{
    if (!modem.isGprsConnected())
    {
        conectarLTE();
    }
}