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
#include "main.h"
#include "config_module.h"
#include "display_module.h"
#include "sensor_module.h"

void setup() {
  delay(1000); /**< Wait until serial terminal has done with platform messages.. */
  app_init();
  config_module_init();
  sensor_module_init();  
  display_module_init();
}

void loop() {     
  static uint32_t cnt = 0;
  delay(1000);    
  display_module_clear();
  display_module_set_reading(((cnt++) & 0x00000001)?1:0);
  sensor_tasker();  
  battery_tasker();
  display_module_update();
}

void app_init()
{
  Serial.begin(APP_SERIAL_BAUDRATE);                                     
  Serial.write("*********************************\r\n");
  Serial.write("* DIY_CO2_MONITOR by Oscar Sanz *\r\n");
  Serial.write("*********************************\r\n\r\n");
}

void sensor_tasker()
{
  uint16_t ppm = sensor_module_ppm_read();
  display_module_set_ppm(ppm);
  display_module_draw_ppm();  
  display_module_draw_ppm_graph();
}

void battery_tasker()
{
  display_module_set_battery(100);
}