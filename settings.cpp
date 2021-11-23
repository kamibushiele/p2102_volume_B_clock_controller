#include "settings.h"

#ifdef _USE_PRIVATE_SSID_
#include "settings_default_private.h"
#else
#include "settings_default.h"
#endif

static Preferences preferences;

setting_t settings_internal;

void showAPSettings(ap_setting_t *s){
    DEBUG_PRINTF("  ssid   = %s\n",s->ssid);
    DEBUG_PRINTF("  pass   = %s\n",s->password);
    DEBUG_PRINTF("  static = %s\n",s->staticIP?"true":"false");
    DEBUG_PRINTF("  ip     = %d.%d.%d.%d\n",s->ip[0],s->ip[1],s->ip[2],s->ip[3]);
    DEBUG_PRINTF("  defGW  = %d.%d.%d.%d\n",s->defaultGW[0],s->defaultGW[1],s->defaultGW[2],s->defaultGW[3]);
    DEBUG_PRINTF("  SM     = %d.%d.%d.%d\n",s->subnetmask[0],s->subnetmask[1],s->subnetmask[2],s->subnetmask[3]);
}

void initSettings(){
    int len;
    uint8_t ssidCharCnt;
    uint8_t i;
    uint32_t chipid;
    bool resetbutton;
    DEBUG_PRINTF("ReadSettings\n");
    // preferences.clear();//削除
    preferences.begin(MODEL_NAME, false);
    len = preferences.getBytes("settings",&settings_internal,sizeof(setting_t));
    DEBUG_PRINTF("len = %d\n",len);
    if(digitalRead(PIN_BTN1) == LOW){
        DEBUG_PRINTF("RESET BUTTON PRESSED\n");
        resetbutton = true;
    }else{
        resetbutton = false;
    }
    if(
        len != sizeof(setting_t) ||
        settings_internal.ap_mode.ssid[0] == '\0' ||
        settings_internal.client_mode.ssid[0] == '\0' || 
        resetbutton
    ){
    // if(true){
        DEBUG_PRINTF("SetDefault\n");
        memcpy(&settings_internal,&settings_default,sizeof(setting_t));
        for(ssidCharCnt=0;ssidCharCnt<26;ssidCharCnt++){
            if(settings_internal.ap_mode.ssid[ssidCharCnt] == '\0'){
                break;
            }
        }
        chipid = getChipID();
        sprintf(settings_internal.ap_mode.ssid+ssidCharCnt,"%06x",chipid);
        DEBUG_PRINTF("macaddress = %06x\n",chipid);
        preferences.putBytes("settings",&settings_internal,sizeof(setting_t));
    }else{
        DEBUG_PRINTF("succes to read!\n");
    }
    DEBUG_PRINTF("---client_mode\n");
    showAPSettings(&(settings_internal.client_mode));
    DEBUG_PRINTF("---ap_mode\n");
    showAPSettings(&(settings_internal.ap_mode));
}

void saveSetting(){
    preferences.putBytes("settings",&settings_internal,sizeof(setting_t));
}

uint32_t getChipID(){
    uint64_t rawMac;
    uint32_t chipId = 0;
    rawMac = ESP.getEfuseMac();
    for(int i=0; i<24; i=i+8) {
        chipId |= ((rawMac >> (40 - i)) & 0xff) << i;
    }
    return chipId;
}

bool assertSSID(String ssid){
    bool res = true;
    char temp[SETTING_AP_SSID_SIZE];
    int i;
    ssid.toCharArray(temp,SETTING_AP_SSID_SIZE);
    i=0;
    while(1){
        if(i > SETTING_AP_SSID_SIZE){
            res = false;
            break;
        }
        if(temp[i] == '\0'){
            if(i == 0){
                res = false;
            }
            break;
        }else if(
            (temp[i] < '0' && temp[i] > '9')&&
            (temp[i] < 'A' && temp[i] > 'Z')&&
            (temp[i] < 'z' && temp[i] > 'z')&&
            temp[i] != ' '&&
            temp[i] != '!'&&
            temp[i] != '#'&&
            temp[i] != '('&&
            temp[i] != ')'&&
            temp[i] != '+'&&
            temp[i] != '-'&&
            temp[i] != '.'&&
            temp[i] != '/'&&
            temp[i] != '%'&&
            temp[i] != '='&&
            temp[i] != '?'&&
            temp[i] != '@'&&
            temp[i] != '^'&&
            temp[i] != '_'&&
            temp[i] != '{'&&
            temp[i] != '|'&&
            temp[i] != '}'
        ){
            res = false;
            break;
        }
        i++;
    }
    return res;
}
bool assertPassword(String password, int minlength){
    return password.length() >= minlength;
}