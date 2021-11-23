#include "rtc.h"

#include "led.h"

Timezone myTZ;
bool manual_time;
bool standalone_mode;

void rtcInit(bool offline) {
#ifdef _DEBUG_
    setDebug(INFO);//デバッグ情報
#else
    setDebug(NONE);//デバッグ情報非表示
#endif
    myTZ.setDefault();
    myTZ.setLocation(String(settings_internal.ntp_tz));
    if(offline){
        setServer("");
        setInterval(0);//not connect ntp server;
        manual_time = true;
    }else{
        setServer(settings_internal.ntp_server);
        setInterval(3600);//second
        manual_time = !waitForSync(30);
        DEBUG_PRINTLN(myTZ.dateTime());
    }
}

led_display_t led = {
    .led = {
        0b1010101010101,
        0b0101010101010,
        0b1010101010101,
        0b0101010101010,
        0b1010101010101,
    }
};
void rtcInLoop(char dot) {
    int h;
    int m;
    int s;
    events();
    if (secondChanged()) {//中でなんらかのezTimeの関数を呼び出さないといけない?
        h = myTZ.hour();
        m = myTZ.minute();
        s = myTZ.second();
        ledMakeDisplay(&led,h,m,dot);
        ledUpdate(&led);
        bring_cnt_clear();
    }
}