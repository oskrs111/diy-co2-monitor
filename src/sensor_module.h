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
#define SENSOR_RX_PIN 25                                          // Rx pin which the MHZ19 Tx pin is attached to
#define SENSOR_TX_PIN 26                                          // Tx pin which the MHZ19 Rx pin is attached to
#define SENSOR_BAUDRATE 9600         
#define SENSOR_MAX_PPM 5000          
#define SENSOR_MIN_PPM 0

#define SENSOR_FLAGS_DEFAULT 0x00
#define SENSOR_FLAGS_AUTOCALIBRATION_ENABLE 0x01

struct sensor_preferences
{
    uint8_t flags;
    uint8_t _align;
};

void sensor_module_init();
void sensor_module_defaults(struct sensor_preferences* preferences);
uint16_t sensor_module_ppm_read();
#endif