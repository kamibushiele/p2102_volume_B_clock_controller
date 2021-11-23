#include "my_html_server.h"

#include <WString.h>
KEServer *server;

String html_header = R"(<html lang="ja"><head><meta charset="UTF-8"><meta name="theme-color" content="#ffd016"><meta name="viewport" content="width=device-width">)";
String html_auto_reoad = R"(<meta http-equiv="refresh" content="5;URL=">)";
String html_footer = R"(</body></html>)";
String html_toTop = R"(<p><a href="/">トップへ</a></p>)";

void page_top(){
    String txt = "";
    char buf[10];
    txt += html_header;
    txt += R"(<title>トップページ | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<h1>トップページ</h1>)";
    txt += R"(<p>)";
    txt += R"(<h2>情報</h2>)";
    txt += R"(<table rules="cols" frame="void"> <tr>)";
    txt += R"(<td>モデル番号</td><td>)" MODEL_NAME R"(</td>)";
    txt += R"(</tr> <tr>)";
    txt += R"(<td>ESP32チップID</td><td>)";
    sprintf(buf,"%06x",getChipID());
    txt += String(buf);
    txt += R"(</td></tr> <tr>)";
    txt += R"(<td>ネットワーク状態</td>)";
    txt += R"(<td>)";
    if(standalone_mode){
        txt += R"(APモード)";
    }else{
        txt += R"(クライアントモード)";
        if(manual_time){
            txt += R"(/NTPは未使用)";
        }else{
            txt += R"(/NTP使用)";
        }
    }
    txt += R"(</td>)";
    txt += R"(</tr> </table>)";
    txt += R"(</p>)";
    txt += R"(<p>)";
    txt += R"(<a href="/s"><h2>ネットワーク設定</h2></a>)";
    txt += R"(</p>)";
    txt += R"(<p>)";
    txt += R"(<a href="/time"><h2>時刻設定</h2></a>)";
    txt += R"(</p>)";
    txt += R"(<p>)";
    txt += R"(<a href="/chime"><h2>外部ボリュームの音量を下げる</h2></a>)";
    txt += R"(</p>)";
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
}
void page_time_setting(){
    String txt = "";
    txt += html_header;
    txt += R"(<title>時刻設定 | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<form method="post">)";
    txt += R"(<h1>NTP設定</h1>)";
    txt += R"(<p>NTPサーバーアドレス<br><input placeholder="" type="text" name="ntp_server" size="20" value=")";
    txt += String(settings_internal.ntp_server); 
    txt += R"("></p>)";
    txt += R"(<p>タイムゾーン(Asia/Tokyo等)<br><input placeholder="" type="text" name="ntp_tz" size="20" value=")";
    txt += String(settings_internal.ntp_tz); 
    txt += R"("></p>)";
    txt += R"(<p>)";
    txt += R"(<input )";
    if(!manual_time)txt += "checked ";
    txt += R"(type="radio" name="manual_time" value="0">NTPを使用する)";
    txt += R"(<input )";
    if(manual_time)txt += "checked ";
    txt += R"(type="radio" name="manual_time" value="1">NTPを使用しない(手動設定))";
    txt += R"(<br>)";
    txt += R"(<input type="number" min="0" max="24" name="h" size="1" value=")";
    txt += String(myTZ.hour());
    txt += R"(">)";
    txt += R"(:<input type="number" min="0" max="59" name="m" size="1" value=")";
    txt += String(myTZ.minute());
    txt += R"(">)";
    txt += R"(:<input type="number" min="0" max="59" name="s" size="1" value=")";
    txt += String(myTZ.second());
    txt += R"(">)";
    txt += "</p>";
    txt += R"(<input checked type="submit" value="適用"> <input type="reset" value="リセット">)";
    txt += R"(</form>)";
    txt += html_toTop;
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
}
void page_time_setting_post(){
    bool new_manual_time;
    String new_ntp_server;
    String new_ntp_tz;
    String txt = "";
    txt += html_header;
    txt += html_auto_reoad;
    txt += R"(<title>時刻設定 | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<p>)";
    txt += R"(設定完了しました。5秒後にリロードされます)";
    txt += R"(</p>)";
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
    new_manual_time = server->web->arg("manual_time").equals("1");
    new_ntp_server = server->web->arg("ntp_server");
    new_ntp_tz = server->web->arg("ntp_tz");
    if(new_manual_time != manual_time){
        manual_time = new_manual_time;
        if(manual_time == true){
            DEBUG_PRINTF("OFFLINE MODE\n");
            setInterval(0);//ntp query disable
        }else{
            DEBUG_PRINTF("NTP MODE\n");
            setInterval(3600);//second
            updateNTP();
        }
    }
    if(
        !new_ntp_server.equals(settings_internal.ntp_server) ||
        !new_ntp_tz.equals(settings_internal.ntp_tz)
    ){
        new_ntp_server.toCharArray(settings_internal.ntp_server,SETTING_NTP_SERVER_SIZE);
        new_ntp_tz.toCharArray(settings_internal.ntp_tz,SETTING_NTP_TZ_SIZE);
        saveSetting();
        if(manual_time == false){
            myTZ.setLocation(new_ntp_tz);
            myTZ.setDefault();
            setServer(new_ntp_server);
            manual_time = !waitForSync(30);
        }
    }
    if(manual_time){
        myTZ.setTime(
            atoi(server->web->arg("h").c_str()),
            atoi(server->web->arg("m").c_str()),
            atoi(server->web->arg("s").c_str()),
            1,0,2021
        );
    }
}
void page_setting(){
    String txt = "";
    uint8_t i;
    uint8_t k;
    txt += html_header;
    txt += R"(<title>ネットワーク設定 | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<form method="post">)";
    txt += R"(<h1>WiFi クライアント設定</h1>)";
    txt += R"(<p>SSID(~32文字)<br><input placeholder="空欄の場合変更なし" type="text" name="ssid" size="20" maxlength="32" value=")";
    txt += String(settings_internal.client_mode.ssid); 
    txt += R"("></p>)";
    txt += R"(<p>パスワード(~63文字)<br><input placeholder="空欄の場合変更なし" type="password" name="password" size="20" value="" maxlength="63" ></p>)";
    txt += R"(<p>)";
    txt += R"(<input )";
    if(settings_internal.client_mode.staticIP)txt += "checked ";
    txt += R"(type="radio" name="staticip" value="1">静的IP)";
    txt += R"(<input )";
    if(!settings_internal.client_mode.staticIP)txt += "checked ";
    txt += R"(type="radio" name="staticip" value="0">DHCP)";
    txt += R"(</p>)";
    for(k = 0; k < 3; k++){
        String title;
        String propname;
        uint8_t *ipadd;
        switch(k){
            case 0:
                title = "<p>IPアドレス<br>";
                propname = "ip";
                ipadd = settings_internal.client_mode.ip;
            break;
            case 1:
                title = "<p>デフォルトゲートウェイ<br>";
                propname = "dgw";
                ipadd = settings_internal.client_mode.defaultGW;
            break;
            default:
                title = "<p>サブネットマスク<br>";
                propname = "snm";
                ipadd = settings_internal.client_mode.subnetmask;
            break;
        }
        txt += title;
        for(i=0;i<4;i++){
            txt += R"(<input type="number" min="0" max="255" name=")";
            txt += String(propname);
            txt += String(i);
            txt += R"(" size="1" value=")";
            txt += String(ipadd[i]);
            txt += R"(">)";
            if(i<3){
                txt += ".";
            }
        }
        txt += "</p>";
    }
    txt += "<hr>";
    txt += R"(<h1>WiFi AP設定</h1>)";
    txt += R"(<p>SSID(~32文字)<br><input placeholder="空欄の場合変更なし" type="text" name="ap_ssid" size="20" maxlength="32" value=")";
    txt += String(settings_internal.ap_mode.ssid); 
    txt += R"("></p>)";
    txt += R"(<p>パスワード(8~63文字)<br><input placeholder="空欄の場合変更なし" type="password" name="ap_password" size="20" value="" minlength="8" maxlength="63"></p>)";
    txt += R"(<p>SSID・パスワード変更時に電源再投入が必要かも</p>)";
    for(k = 0; k < 3; k++){
        String title;
        String propname;
        uint8_t *ipadd;
        switch(k){
            case 0:
                title = "<p>IPアドレス<br>";
                propname = "ap_ip";
                ipadd = settings_internal.ap_mode.ip;
            break;
            case 1:
                title = "<p>デフォルトゲートウェイ<br>";
                propname = "ap_dgw";
                ipadd = settings_internal.ap_mode.defaultGW;
            break;
            default:
                title = "<p>サブネットマスク<br>";
                propname = "ap_snm";
                ipadd = settings_internal.ap_mode.subnetmask;
            break;
        }
        txt += title;
        for(i=0;i<4;i++){
            txt += R"(<input type="number" min="0" max="255" name=")";
            txt += String(propname);
            txt += String(i);
            txt += R"(" size="1" value=")";
            txt += String(ipadd[i]);
            txt += R"(">)";
            if(i<3){
                txt += ".";
            }
        }
        txt += "</p>";
    }
    txt += R"(<input checked type="submit" value="更新して再起動"> <input type="reset" value="リセット">)";
    txt += R"(</form>)";
    txt += html_toTop;
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
}
void page_setting_post(){
    String txt = "";
    String temp;
    txt += html_header;
    txt += R"(<title>ネットワーク設定 | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<p>)";
    txt += R"(設定完了しました。3秒後に再起動されます)";
    txt += R"(</p>)";
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
    
    temp = server->web->arg("ssid");
    if(assertSSID(temp))temp.toCharArray(settings_internal.client_mode.ssid,SETTING_AP_SSID_SIZE);
    temp = server->web->arg("password");
    if(assertPassword(temp,1))temp.toCharArray(settings_internal.client_mode.password,SETTING_AP_PASSWORD_SIZE);
    settings_internal.client_mode.staticIP          = server->web->arg("staticip").equals("1");
    settings_internal.client_mode.ip[0]             = server->web->arg("ip0").toInt();
    settings_internal.client_mode.ip[1]             = server->web->arg("ip1").toInt();
    settings_internal.client_mode.ip[2]             = server->web->arg("ip2").toInt();
    settings_internal.client_mode.ip[3]             = server->web->arg("ip3").toInt();
    settings_internal.client_mode.defaultGW[0]      = server->web->arg("dgw0").toInt();
    settings_internal.client_mode.defaultGW[1]      = server->web->arg("dgw1").toInt();
    settings_internal.client_mode.defaultGW[2]      = server->web->arg("dgw2").toInt();
    settings_internal.client_mode.defaultGW[3]      = server->web->arg("dgw3").toInt();
    settings_internal.client_mode.subnetmask[0]     = server->web->arg("snm0").toInt();
    settings_internal.client_mode.subnetmask[1]     = server->web->arg("snm1").toInt();
    settings_internal.client_mode.subnetmask[2]     = server->web->arg("snm2").toInt();
    settings_internal.client_mode.subnetmask[3]     = server->web->arg("snm3").toInt();
    temp = server->web->arg("ap_ssid");
    if(assertSSID(temp))temp.toCharArray(settings_internal.ap_mode.ssid,SETTING_AP_SSID_SIZE);
    temp = server->web->arg("ap_password");
    if(assertPassword(temp,8))temp.toCharArray(settings_internal.ap_mode.password,SETTING_AP_PASSWORD_SIZE);
    settings_internal.ap_mode.staticIP      = true;
    settings_internal.ap_mode.ip[0]         = server->web->arg("ap_ip0").toInt();
    settings_internal.ap_mode.ip[1]         = server->web->arg("ap_ip1").toInt();
    settings_internal.ap_mode.ip[2]         = server->web->arg("ap_ip2").toInt();
    settings_internal.ap_mode.ip[3]         = server->web->arg("ap_ip3").toInt();
    settings_internal.ap_mode.defaultGW[0]  = server->web->arg("ap_dgw0").toInt();
    settings_internal.ap_mode.defaultGW[1]  = server->web->arg("ap_dgw1").toInt();
    settings_internal.ap_mode.defaultGW[2]  = server->web->arg("ap_dgw2").toInt();
    settings_internal.ap_mode.defaultGW[3]  = server->web->arg("ap_dgw3").toInt();
    settings_internal.ap_mode.subnetmask[0] = server->web->arg("ap_snm0").toInt();
    settings_internal.ap_mode.subnetmask[1] = server->web->arg("ap_snm1").toInt();
    settings_internal.ap_mode.subnetmask[2] = server->web->arg("ap_snm2").toInt();
    settings_internal.ap_mode.subnetmask[3] = server->web->arg("ap_snm3").toInt();
    saveSetting();
    DEBUG_PRINTF("REBOOT!");
    delay(3000);
    ESP.restart();
}

void page_chime(){
    String txt = "";
    String temp;
    txt += html_header;
    txt += R"(<title>チャイム検出 | )" MODEL_NAME R"(</title></head><body>)";
    txt += R"(<p>)";
    txt += R"(音量を下げます)";
    txt += R"(</p>)";
    txt += html_footer;
    server->web->send(200, "text/html", txt.c_str());
    OIS_PRINTF("\nVOLDOWN\n");
}

void volmeServerInit(){
    server = new KEServer(80);
    server->web->on("/", HTTP_GET, page_top);
    server->web->on("/time", HTTP_GET, page_time_setting);
    server->web->on("/time", HTTP_POST, page_time_setting_post);
    server->web->on("/s", HTTP_GET, page_setting);
    server->web->on("/s", HTTP_POST, page_setting_post);
    server->web->on("/chime", HTTP_GET, page_chime);
    server->start();
}
