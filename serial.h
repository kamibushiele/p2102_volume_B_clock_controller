#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "common.h"

#define SLICE_CHAR '\n'
#define RB_MAX (32)

#define OIS_PRINTF(...) Serial.printf(__VA_ARGS__)

void serialRead();
void serialInit();
void Decode(char *rb, uint8_t cur);

#endif