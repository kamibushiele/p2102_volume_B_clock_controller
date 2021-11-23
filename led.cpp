#include "led.h"

void ledSet(uint16_t a,uint8_t k);
void ARDUINO_ISR_ATTR timLEDISR();

static hw_timer_t *tim_LED_h = NULL;  //ハンドラ
static volatile SemaphoreHandle_t tim_LED_sem; //セマフォ
static portMUX_TYPE tim_LED_mux = portMUX_INITIALIZER_UNLOCKED;//排他制御
static bool state_update_f = false;
static bool bring_cnt_clear_f = false;
static uint16_t bring_cnt = 0;
static bool bright_read_f = false;
static led_display_t internal_led;
static led_display_t *next_led;
static uint8_t led_bright = 100;
static const uint16_t led_font[] = {
    075557,//0
    022222,//1
    071747,//2
    071717,//3
    055711,//4
    074717,//5
    074757,//6
    071111,//7
    075757,//8
    075717,//9
};

void ledUpdate(led_display_t *led){
    next_led = led;
    state_update_f = true;
}
void bring_cnt_clear(){
    bring_cnt_clear_f = true;
}
void ledInLoop(){
    int bright_ad;
    if (xSemaphoreTake(tim_LED_sem, 0) == pdTRUE){
        portENTER_CRITICAL_ISR(&tim_LED_mux);
        if(state_update_f){// メインループ側フラグ判定
            state_update_f = false;
            memcpy(&internal_led,next_led,sizeof(led_display_t));
        }
        if(bring_cnt_clear_f){// メインループ側フラグ判定
            bring_cnt_clear_f = false;
            bring_cnt = 0;
        }
        if(bright_read_f){// 割り込み内部側のフラグ判定
            bright_read_f = false;
            bright_ad = analogRead(PIN_BRIGHT);
            if(bright_ad >= 2500){
                led_bright = 100;
            }else if(bright_ad >= 1500){
                led_bright = 64;
            }else if(bright_ad >= 1000){
                led_bright = 32;
            }else if(bright_ad >= 500){
                led_bright = 16;
            }else if(bright_ad >= 250){
                led_bright = 8;
            }else if(bright_ad >= 100){
                led_bright = 4;
            }else if(bright_ad >= 50){
                led_bright = 2;
            }else{
                led_bright = 1;
            }
        }
        portEXIT_CRITICAL_ISR(&tim_LED_mux);
    }
}


void ledMakeDisplay(led_display_t *led,uint8_t h,uint8_t m,char dot){
    for(int i=0; i<5; i++){
        led->led[i] = 0;
        led->led[i] |= ((led_font[m%10]>>12-(3*i))&(0b111))<<0;
        led->led[i] |= ((led_font[m/10]>>12-(3*i))&(0b111))<<3;
        led->led[i] |= ((led_font[h%10]>>12-(3*i))&(0b111))<<7;
        led->led[i] |= ((led_font[h/10]>>12-(3*i))&(0b111))<<10;
    }
    switch(dot){
        case ':':
            led->led[1] |= 0b0000001000000;
            led->led[3] |= 0b0000001000000;
        break;
        case ';':
            led->led[1] |= 0b0000001000000;
            led->led[3] |= 0b0000001000000;
            led->led[4] |= 0b0000001000000;
        break;
        case '.':
            led->led[4] |= 0b0000001000000;
        break;
        case ',':
            led->led[3] |= 0b0000001000000;
            led->led[4] |= 0b0000001000000;
        break;
    }
}

// ---------------------------------------------------------------------
void ARDUINO_ISR_ATTR timLEDISR() {//割り込み
    static uint8_t cnt = 0;
    static uint8_t cnt_k = 0;
    static uint8_t cnt_ms = 0;
    portENTER_CRITICAL_ISR(&tim_LED_mux);
    cnt++;
    if (cnt >= led_bright) {
        ledSet(0, 0xff);
    }
    if (cnt > 100) {
        cnt = 0;
        if(bring_cnt >= 500){
            internal_led.led[1] &= 0b1111110111111;
            internal_led.led[3] &= 0b1111110111111;
            internal_led.led[4] &= 0b1111110111111;
        }else{
            bring_cnt ++;
        }
        ledSet(internal_led.led[cnt_k], cnt_k);
        if(cnt_k >= 4){
            cnt_k = 0;
        }else{
            cnt_k++;
        }
        if(cnt_ms > 100){//read bright
            bright_read_f = true;
            cnt_ms = 0;
        }else{
            cnt_ms ++;
        }
    }
    portEXIT_CRITICAL_ISR(&tim_LED_mux);
    xSemaphoreGiveFromISR(tim_LED_sem, NULL);
}
void ledInit() {
    tim_LED_sem = xSemaphoreCreateBinary();

    tim_LED_h = timerBegin(3, 80, true);                //タイマー0-3 , プリスケーラー , インクリメント
    timerAttachInterrupt(tim_LED_h, &timLEDISR, true);  //ハンドラ, 呼び出し関数, エッジタイプ
    timerAlarmWrite(tim_LED_h, 10, true);               //ハンドラ, ポストスケーラ, 繰り返し有効
    timerAlarmEnable(tim_LED_h);                        //開始
}
void ledSet(uint16_t a, uint8_t k) {
    digitalWrite(PIN_K1, k == 0 ? HIGH : LOW);
    digitalWrite(PIN_K2, k == 1 ? HIGH : LOW);
    digitalWrite(PIN_K3, k == 2 ? HIGH : LOW);
    digitalWrite(PIN_K4, k == 3 ? HIGH : LOW);
    digitalWrite(PIN_K5, k == 4 ? HIGH : LOW);

    digitalWrite(PIN_A4_3, a & 0x0001 ? HIGH : LOW);
    digitalWrite(PIN_A4_2, a & 0x0002 ? HIGH : LOW);
    digitalWrite(PIN_A4_1, a & 0x0004 ? HIGH : LOW);
    digitalWrite(PIN_A3_3, a & 0x0008 ? HIGH : LOW);
    digitalWrite(PIN_A3_2, a & 0x0010 ? HIGH : LOW);
    digitalWrite(PIN_A3_1, a & 0x0020 ? HIGH : LOW);
    digitalWrite(PIN_AD_1, a & 0x0040 ? HIGH : LOW);
    digitalWrite(PIN_A2_3, a & 0x0080 ? HIGH : LOW);
    digitalWrite(PIN_A2_2, a & 0x0100 ? HIGH : LOW);
    digitalWrite(PIN_A2_1, a & 0x0200 ? HIGH : LOW);
    digitalWrite(PIN_A1_3, a & 0x0400 ? HIGH : LOW);
    digitalWrite(PIN_A1_2, a & 0x0800 ? HIGH : LOW);
    digitalWrite(PIN_A1_1, a & 0x1000 ? HIGH : LOW);
}