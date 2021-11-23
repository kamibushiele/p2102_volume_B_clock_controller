#include "led.h"
#include "serial.h"
#include "rtc.h"
#include "settings.h"
#include "my_html_server.h"
#include <WiFi.h>

#define PIN_SW 34
#define PIN_BRIGHT 35

void setup() {
    int i = 0;
    serialInit();//最初
    initGPIO();
    initSettings();//設定読み込み
    if(settings_internal.client_mode.staticIP){
        WiFi.config(
            IPAddress(settings_internal.client_mode.ip),
            IPAddress(settings_internal.client_mode.defaultGW),
            IPAddress(settings_internal.client_mode.subnetmask),
            IPAddress(settings_internal.client_mode.defaultGW)//DNS server
        );
    }
    WiFi.begin(settings_internal.client_mode.ssid, settings_internal.client_mode.password);
    standalone_mode = false;
    while (WiFi.status() != WL_CONNECTED) {
        if(i >= 20){
            standalone_mode = true;
            break;
        }
        i++;
        delay(500);
        DEBUG_PRINTF(".");
    }
    if(!standalone_mode){
        DEBUG_PRINTF("connect!\n");
        DEBUG_PRINTLN(WiFi.localIP());
        DEBUG_PRINTLN(WiFi.macAddress());
        DEBUG_PRINTLN(WiFi.subnetMask());
        DEBUG_PRINTLN(WiFi.gatewayIP());
        DEBUG_PRINTLN(WiFi.dnsIP());
    }else{// Standalone mode
        DEBUG_PRINTF("standalone mode!\n");
        WiFi.disconnect(true,true);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(settings_internal.ap_mode.ssid,settings_internal.ap_mode.password);
        delay(100);//SYSTEM_EVENT_AP_STARTを待つ
        WiFi.softAPConfig(
            IPAddress(settings_internal.ap_mode.ip),
            IPAddress(settings_internal.ap_mode.defaultGW),
            IPAddress(settings_internal.ap_mode.subnetmask)
        );
        DEBUG_PRINTLN(WiFi.softAPIP());//起動も兼ねる
    }
    rtcInit(standalone_mode);
    volmeServerInit();
    ledInit();//wifi設定より後に設定する
    DEBUG_PRINTF("INIT END\n");
}
void loop() {
    char dot;
    server->inLoop();
    ledInLoop();
    serialRead();
    if(standalone_mode){
        dot = '.';
    }else{
        if(manual_time){
            dot = ';';
        }else{
            dot = ':';
        }
    }
    rtcInLoop(dot);
}