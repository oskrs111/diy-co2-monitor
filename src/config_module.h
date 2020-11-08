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
#ifndef CONFIG_MODULE_H
#define CONFIG_MODULE_H
#define PREFERENCES_NAMESPACE "preferences"
#define PREFERENCES_MAX_SIZE 128
#define PREFERENCES_SIGNATURE 0x11AA22BB
//Include here additional modules headers
#include "sensor_module.h"
#include "display_module.h"

struct preferences_common
{
    uint32_t signature;
};

struct preferences_group
{
    struct preferences_common common;
    //Include here additional modules structures
    struct sensor_preferences sensor;
    struct display_preferences display;
}; 

union preferences_union
{       
    struct preferences_group preferences;
    uint8_t data[PREFERENCES_MAX_SIZE];
};

void config_module_init();
void config_module_set_defaults();
void config_module_defaults(struct preferences_common* preferences);
struct preferences_group* config_module_get_preferences();
#endif