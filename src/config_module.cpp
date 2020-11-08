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
#include <Preferences.h>
#include "config_module.h"

static union preferences_union preferences;
static Preferences pref_interface;

void config_module_init()
{
    pref_interface.begin(PREFERENCES_NAMESPACE, false);    
    uint32_t signature = pref_interface.getULong(PREFERENCES_NAMESPACE, 0);
    if(signature != PREFERENCES_SIGNATURE)
    {
        config_module_set_defaults();
        Serial.write("> config_module_init() => Config defaults!.\r\n");
    }
    else
    {
        Serial.write("> config_module_init() => Config succesfuly loaded.\r\n");
    }
            
    pref_interface.getBytes(PREFERENCES_NAMESPACE, &preferences.data[0], sizeof(union preferences_union));    
}

void config_module_set_defaults()
{
    memset(&preferences, 0x00, sizeof(union preferences_union));
    config_module_defaults(&preferences.preferences.common);
    sensor_module_defaults(&preferences.preferences.sensor);
    display_module_defaults(&preferences.preferences.display);
}

void config_module_defaults(struct preferences_common* preferences)
{
    preferences->signature = PREFERENCES_SIGNATURE;
}

struct preferences_group* config_module_get_preferences()
{
    return &preferences.preferences;
}





