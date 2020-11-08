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
#include <Arduino.h>
#include <SoftwareSerial.h>                                
#include "sensor_module.h"
#include "config_module.h"
#include "MHZ19.h"
static SoftwareSerial serial(SENSOR_RX_PIN, SENSOR_TX_PIN);                   
static struct sensor_preferences* preferences = 0x00;
static MHZ19 sensor;
void sensor_module_init()
{  
    preferences = &config_module_get_preferences()->sensor;
    serial.begin(SENSOR_BAUDRATE);                                    
    sensor.begin(serial);                                
    if(preferences > 0x00)
    {
        sensor.autoCalibration((preferences->flags & SENSOR_FLAGS_AUTOCALIBRATION_ENABLE)?1:0);  
    }    
    Serial.write("> sensor_module_init() => Done!\r\n");
}

void sensor_module_defaults(struct sensor_preferences* preferences)
{
    preferences->flags = SENSOR_FLAGS_DEFAULT;        
}

uint16_t sensor_module_ppm_read()
{
    return sensor.getCO2(true, true);
}
