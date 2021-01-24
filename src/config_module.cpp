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
#include <Preferences.h>
#include "config_module.h"

static union preferences_union preferences;
static Preferences pref_interface;

void config_module_init()
{
    pref_interface.begin(PREFERENCES_NAMESPACE, false);    
    //uint32_t signature = pref_interface.getULong(PREFERENCES_NAMESPACE, 0);
    pref_interface.getBytes(PREFERENCES_NAMESPACE, &preferences.data[0], sizeof(union preferences_union));    
    Serial.print("> config_module_init(signature) => ");
    Serial.println(preferences.preferences.common.signature);
    if(preferences.preferences.common.signature != PREFERENCES_SIGNATURE)
    {
        config_module_set_defaults();
        Serial.println("> config_module_init() => Config defaults!.");
    }
    else
    {
        Serial.println("> config_module_init() => Config succesfuly loaded.");
    }            
}

void config_module_set_defaults()
{
    memset(&preferences, 0x00, sizeof(union preferences_union));
    config_module_defaults(&preferences.preferences.common);
    sensor_module_defaults(&preferences.preferences.sensor);
    display_module_defaults(&preferences.preferences.display);
    wifi_module_defaults(&preferences.preferences.wifi);    
    pref_interface.putBytes(PREFERENCES_NAMESPACE, &preferences.data[0], sizeof(union preferences_union));    
    delay(500);
}

void config_module_defaults(struct preferences_common* preferences)
{
    preferences->signature = PREFERENCES_SIGNATURE;
}

struct preferences_group* config_module_get_preferences()
{
    return &preferences.preferences;
}

void config_module_set_parameter(char* parameter, void* value)
/**< This is not so eficient but efective... */
{
    uint8_t len = 0x00;
    if(parameter == 0x00) return;    
    Serial.print("> config_module_set_parameter(char* parameter, void* value) => ");
    Serial.print(parameter);
    Serial.print(" ,");
    
    if(strcmp("wifi.ssid", parameter) == 0x00)
    {
        len = (strlen((char*)value) < sizeof(preferences.preferences.wifi.ssid))?strlen((char*)value):sizeof(preferences.preferences.wifi.ssid);
        memcpy(&preferences.preferences.wifi.ssid[0], (char*)value, len);
        preferences.preferences.wifi.ssid[len] = 0x00;
        preferences.preferences.wifi.flags |= WIFI_FLAGS_ENABLE;
        Serial.print((char*)&preferences.preferences.wifi.ssid[0]);
    }
    
    if(strcmp("wifi.password", parameter) == 0x00)
    {
        len = (strlen((char*)value) < sizeof(preferences.preferences.wifi.password))?strlen((char*)value):sizeof(preferences.preferences.wifi.password);
        memcpy(&preferences.preferences.wifi.password[0], (char*)value, len);
        preferences.preferences.wifi.password[len] = 0x00;
        preferences.preferences.wifi.flags |= WIFI_FLAGS_ENABLE;
        Serial.print((char*)&preferences.preferences.wifi.password[0]);
    }    
#ifdef BUZZER_MODULE
    if(strcmp("ppm.alarm_value", parameter) == 0x00)
    {
        preferences.preferences.sensor.alarm_value = atoi((char*)value);
        Serial.print(preferences.preferences.sensor.alarm_value);
    }    
#endif    
    Serial.println();
    if( pref_interface.putBytes(PREFERENCES_NAMESPACE, &preferences.data[0], sizeof(union preferences_union)) > 0x00)
    {
        Serial.println("> config_module_set_parameter(char* parameter, void* value) => Peferences saved.");
    }
}





