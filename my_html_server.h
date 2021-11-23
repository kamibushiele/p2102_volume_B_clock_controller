#ifndef _MY_HTML_SERVER_H_
#define _MY_HTML_SERVER_H_

#include "common.h"

#include <WiFi.h>
#include <WebServer.h>
#include <string>
using namespace std;

#include "settings.h"
#include "rtc.h"
#include "serial.h"


class KEServer{
    public:
    WebServer *web;
    KEServer(int port){
        web = new WebServer(port);
    }
    void start() {
        web->begin();
    }
    void inLoop(){
        web->handleClient();
    }
};
extern KEServer *server;
void volmeServerInit();

#endif