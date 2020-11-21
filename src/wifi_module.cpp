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
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include "wifi_module.h"
#include "config_module.h"  

static struct wifi_preferences* preferences = 0x00;

void wifi_module_init() 
{
    preferences = &config_module_get_preferences()->wifi;
    Serial.print("> WIFI Params: ");
    Serial.print(&preferences->ssid[0]);
    Serial.print(", ");
    Serial.println(&preferences->password[0]);
        
    if(preferences->flags & WIFI_FLAGS_ENABLE)
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(preferences->ssid, preferences->password);
        if (WiFi.waitForConnectResult() != WL_CONNECTED) {
            Serial.printf("> WiFi Failed!\n");
            return;
        }
        Serial.print("> WIFI IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        WiFi.mode(WIFI_OFF);
        WiFi.begin();
        Serial.println("> WIFI is disabled. ");
    }
}

void wifi_module_defaults(struct wifi_preferences* preferences)
{
    memset(preferences, 0x00, sizeof(struct wifi_preferences));    
    memcpy(&preferences->ssid[0], "SSID", strlen("SSID"));
    memcpy(&preferences->password[0], "PASSWORD", strlen("PASSWORD"));
}

char* wifi_module_get_ipaddress(uint8_t dots)
{
    static unsigned char ip[16] ={'0','0','0','.','0','0','0','.','0','0','0','.','0','0','0',0x00};
    uint8_t t = 0;

    if((preferences->flags & WIFI_FLAGS_ENABLE) == 0x00)
    {
        return (char*)"W.OFF";
    }

    WiFi.localIP().toString().getBytes(&ip[0], sizeof(ip), 0x00);
    if(ip[0] == '0')
    {
        return (char*)"W.ERR";
    }

    if(dots < 1) dots = 1;
    if(dots >= 4)
    {
        return (char*)&ip[0];    
    }    

    dots--;
    for(t = (sizeof(ip)-1); t > 0; t--)
    {
        if((ip[t] == '.') && (dots == 0x00))
        {
            return (char*)&ip[t+1];
        }
        else if(ip[t] == '.')
        {
            dots--;
        }
    }
    return (char*)&ip[0];
}