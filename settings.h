#ifndef _SETTING_H_
#define _SETTING_H_

#include "common.h"
#include <Preferences.h>

#include "gpio.h"

#define SETTING_AP_SSID_SIZE (33)
#define SETTING_AP_PASSWORD_SIZE (64)

#define SETTING_NTP_SERVER_SIZE (64)
#define SETTING_NTP_TZ_SIZE (32)

typedef struct{
    char ssid[SETTING_AP_SSID_SIZE];
    char password[SETTING_AP_PASSWORD_SIZE];
    bool staticIP;
    uint8_t ip[4];
    uint8_t defaultGW[4];
    uint8_t subnetmask[4];
} ap_setting_t;

typedef struct{
    char ntp_server[SETTING_NTP_SERVER_SIZE];
    char ntp_tz[SETTING_NTP_TZ_SIZE];
    ap_setting_t client_mode;
    ap_setting_t ap_mode;
} setting_t;

extern setting_t settings_internal;
void initSettings();
void saveSetting();
uint32_t getChipID();
bool assertSSID(String ssid);
bool assertPassword(String password,int minlength);


#endif