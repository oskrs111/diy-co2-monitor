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
#include "timers_module.h"
#include "config_module.h"
#include "display_module.h"
#include "sensor_module.h"
#include "wifi_module.h"
#include "wserver_module.h"
#include "ble_module.h"
#ifdef BUZZER_MODULE
#include "buzzer_module.h"
#endif

void setup() {  
  app_init();
  config_module_init();
  display_module_init();  
  sensor_module_init();    
  wifi_module_init();
  wserver_module_init(); 
  ble_module_init();
  timers_module_init();
  #ifdef BUZZER_MODULE    
  buzzer_module_init();
  buzzer_module_set(BUZZER_ON);
  delay(1000);    
  buzzer_module_set(BUZZER_OFF);
  #else
  delay(1000);    
  #endif    
}

void loop() {       
  
  if(timers_module_test(tid_500ms) > 0x00)
  {    
  } 

  if(timers_module_test(tid_1000ms) > 0x00)
  {
    sensor_tasker();    
    display_module_clear();
    display_tasker();    
    display_module_update();
    #ifdef BUZZER_MODULE    
    buzzer_tasker();
    #endif
  }
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
  static uint8_t cnt = 0x00;
  if(cnt++ & 0x01)
  {
    display_module_set_temperature(sensor_module_temperature_read());
  }
  else  
  {    
    display_module_set_ppm(sensor_module_ppm_read());
  }
}

void display_tasker()
{
  static uint8_t cnt = 0;
  display_module_draw_ppm();  
  if(sensor_module_warming() == 0x00)
  /**< Do not graph the values until the sensor warming has finished */
  {
    display_module_draw_ppm_graph();
  }
  display_module_set_reading(((cnt++) & 0x01)?1:0);
  display_module_set_warming((sensor_module_warming() & cnt)?1:0);
  display_module_set_ipaddress(wifi_module_get_ipaddress(2));
}

void battery_tasker()
{
  display_module_set_battery(100);
}

#ifdef BUZZER_MODULE    
void buzzer_tasker()
{
  if(sensor_module_ppm_get() > config_module_get_preferences()->sensor.alarm_value)  
  {
    buzzer_module_set(BUZZER_ON);
  }
  else
  {
    buzzer_module_set(BUZZER_OFF);
  }
}
#endif