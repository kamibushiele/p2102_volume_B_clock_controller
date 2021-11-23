#ifndef _COMMON_H_
#define _COMMON_H_
#include <Arduino.h>


#define MODEL_NAME "KE-CLK1"

// #define _DEBUG_
// #define _USE_PRIVATE_SSID_ //開発用　開発環境のSSID・パスワード(非追跡ファイル)を使用

#ifdef _DEBUG_
#define DEBUG_PRINTLN(txt) Serial.println(txt)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINTLN(...) 
#define DEBUG_PRINTF(...)
#endif

#endif