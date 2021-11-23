#include "serial.h"

static char serial_RB[RB_MAX] = {SLICE_CHAR};
static uint8_t cursor = 1;      //リングバッファのカーソル
static uint8_t last_slice = 0;  //前回のスライス文字位置のカーソル

void serialInit() {
    Serial.begin(4800);
    delay(10);//いる?
}
void serialRead() {
    int serial_bufferlen;
    if (serial_bufferlen = Serial.available()) {
        for (int i = 0; i < serial_bufferlen; i++) {
            serial_RB[cursor] = Serial.read();
            if (serial_RB[cursor] == SLICE_CHAR) {
                Decode(serial_RB, cursor);
                last_slice = cursor;
            } else if (cursor == last_slice) {
                DEBUG_PRINTF("Buffer OUT\n");
                last_slice = RB_MAX;  //範囲外へ
            }
            cursor++;
            if (cursor >= RB_MAX) {
                cursor = 0;
            }
        }
    }
}

void Decode(char *rb, uint8_t cur) {
    char buf[RB_MAX+1];
    uint8_t read_cursor;
    int i;
    if (last_slice < RB_MAX) {
        for (
                (i = 0, read_cursor = last_slice+1);
                ;
                (i++, read_cursor++)
            ){
            if (read_cursor >= RB_MAX) {
                read_cursor = 0;
            }
            if (read_cursor != cur) {
                buf[i] = rb[read_cursor];
            } else {
                buf[i] = '\0';
                break;
            }
        }
        DEBUG_PRINTF("\"%s\"\n",buf);
    }
}