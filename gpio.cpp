#include "gpio.h"

void initGPIO(){
    pinMode(PIN_A1_1, OUTPUT);
    pinMode(PIN_A1_2, OUTPUT);
    pinMode(PIN_A1_3, OUTPUT);
    pinMode(PIN_A2_1, OUTPUT);
    pinMode(PIN_A2_2, OUTPUT);
    pinMode(PIN_A2_3, OUTPUT);
    pinMode(PIN_AD_1, OUTPUT);
    pinMode(PIN_A3_1, OUTPUT);
    pinMode(PIN_A3_2, OUTPUT);
    pinMode(PIN_A3_3, OUTPUT);
    pinMode(PIN_A4_1, OUTPUT);
    pinMode(PIN_A4_2, OUTPUT);
    pinMode(PIN_A4_3, OUTPUT);
    pinMode(PIN_K1, OUTPUT);
    pinMode(PIN_K2, OUTPUT);
    pinMode(PIN_K3, OUTPUT);
    pinMode(PIN_K4, OUTPUT);
    pinMode(PIN_K5, OUTPUT);

    pinMode(PIN_BTN1,INPUT);

    ledSet(0,0);
}