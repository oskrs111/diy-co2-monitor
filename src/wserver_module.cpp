/*
Copyright 2020 Oscar Sanz Llopis
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <string.h>
#include <sstream>
#include <iostream>
#include "wserver_module.h"
#include "sensor_module.h"
#include "display_module.h"
#include "../www/index.h"
static AsyncWebServer server(80);

void wserver_module_init() 
{    
    server.on("/", HTTP_GET, wserver_root_request);
    server.on("/data", HTTP_GET, wserver_get_request);
    server.on("/set", HTTP_POST, wserver_post_request);
    server.onNotFound(wserver_not_found);
    server.begin();
}

void wserver_root_request(AsyncWebServerRequest *request){
    request->send(200, "text/html", html_buffer);
}

void wserver_get_request(AsyncWebServerRequest *request){
    float temp = sensor_module_temperature_get();
    uint16_t ppm = sensor_module_ppm_get();    
    uint16_t warming = sensor_module_warming();
    
    String data = "{\"ppm\":" + 
                    String(ppm) +
                    ",\"temp\":" +
                    String(temp) +
                    ",\"warming\":" +
                    String(warming) +                   
                    "," + display_module_historical_2_json() +
                    "}";

    request->send(200, "application/json", data);    
}

void wserver_post_request(AsyncWebServerRequest *request){
/*    
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
        message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
        message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
*/
}

void wserver_not_found(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

String wserver_toString(char* type, void* val){
    static char buff[16];
    uint8_t l = 0x00;

    memset(&buff[0], 0x00, sizeof(buff));
    switch(type[1])
    {
        case 'd':            
            l = sprintf(&buff[0],type,*((int*)val));
            break;

        case 'f':            
            l = sprintf(&buff[0],type,*((float*)val));
            break;            

    default:
        return "Unknown type";
    }
    
    buff[l] = 0;
    return String(buff);
}