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
#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H
#define SENSOR_TRACES_ENABLE 0
#define SENSOR_RX_PIN 25                                          // Rx pin which the MHZ19 Tx pin is attached to
#define SENSOR_TX_PIN 26                                          // Tx pin which the MHZ19 Rx pin is attached to
#define SENSOR_BAUDRATE 9600         
#define SENSOR_MAX_PPM 5000          
#define SENSOR_MIN_PPM 0
#define SENSOR_WARMING_TIME (180/2) /**< 3min, sccording to MH-Z19B datasheet. /2 because ppm read is called each 2 secons*/
#define SENSOR_PMM_DIFF_MAX 100
#define SENSOR_TEMP_DIFF_MAX 2
#define SENSOR_MAX_ERROR 10

#define SENSOR_FLAGS_DEFAULT 0x00
#define SENSOR_FLAGS_AUTOCALIBRATION_ENABLE 0x01

struct sensor_preferences
{
    uint8_t flags;
    uint8_t _align;
};

void sensor_module_init();
void sensor_module_defaults(struct sensor_preferences* preferences);
uint16_t sensor_module_ppm_get();
float sensor_module_temperature_get();
uint16_t sensor_module_ppm_read();
float sensor_module_temperature_read();
uint8_t sensor_module_warming();
void sensor_module_check_health();
#endif