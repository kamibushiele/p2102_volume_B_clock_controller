#ifndef _LED_H_
#define _LED_H_

#include "common.h"
#include "gpio.h"

typedef struct {
    uint16_t led[5];
}led_display_t;

void ledUpdate(led_display_t *led);
void bring_cnt_clear();
void ledInLoop();
void ledInit();
void ledMakeDisplay(led_display_t *led,uint8_t h,uint8_t m,char dot);
void ledSet(uint16_t a, uint8_t k);

#endif