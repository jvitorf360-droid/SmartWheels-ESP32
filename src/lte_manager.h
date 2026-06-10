#ifndef LTE_MANAGER_H
#define LTE_MANAGER_H

#define TINY_GSM_MODEM_SIM7600

#include <TinyGsmClient.h>

extern HardwareSerial SerialAT;
extern TinyGsm modem;

bool inicializarLTE();
bool conectarLTE();
bool lteConectado();
void verificarLTE();

#endif