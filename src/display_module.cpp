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
#include <stdint.h>
#include "module_common.h"  
#include "display_module.h"  
#include "display_module_fonts.h"  
#include "SSD1306Wire.h"  
static SSD1306Wire display(0x3c, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
static struct st_display_data display_data;
static char display_string[64];
void display_module_init()
{
    memset(&display_data, 0x00, sizeof(display_data));
    display_module_historical_init();
    display.init();
    display.setI2cAutoInit(true);
    display.flipScreenVertically();    
    display_module_set_battery(100);
    display_module_set_ppm(1234);    
    display_module_set_warming(0);
    display_module_set_reading(0);
    display_module_set_calibrating(0);
    display_module_set_ble(0);
    Serial.write("> display_module_init() => Done!\r\n");
}

void display_module_historical_init()
{
    uint16_t t = 0;
    for(t = 1; t < (DISPLAY_MODULE_HISTORICAL_SPAN - 1); t++)
    {        
        display_data.historical.historical_values[t].p_next = &display_data.historical.historical_values[t+1];
        display_data.historical.historical_values[t].p_prev = &display_data.historical.historical_values[t-1];
        display_data.historical.historical_values[t]._pos = t;        
    }

    display_data.historical.historical_values[0].p_next = &display_data.historical.historical_values[1];
    display_data.historical.historical_values[0].p_prev = &display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)];
    display_data.historical.historical_values[0]._pos = 0;

    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)].p_next = &display_data.historical.historical_values[0];
    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)].p_prev = &display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 2)];
    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)]._pos = (DISPLAY_MODULE_HISTORICAL_SPAN - 1);

    display_data.historical.p_ppm_in = &display_data.historical.historical_values[0];    
    display_data.historical.ppm_length = 0;
    display_data.historical.ppm_max = 5000;
    display_data.historical.ppm_min = 0;
}

void display_module_clear()
{
    display.clear();    
}

void display_module_update()
{
    display.display();    
}

void display_module_set_ppm(uint16_t ppm)
{
    display_data.ppm = ppm;
    display_module_push_historical(ppm);
}

void display_module_set_battery(uint16_t battery)
{
    display_data.battery = battery;
}

void display_module_push_historical(uint16_t ppm)
{
    static uint16_t span_average[DISPLAY_MODULE_SPAN_AVERAGE];
    static uint16_t span_average_count = 0;
#if DEBUG_TRACES_ENABLE
    char string[64];    
#endif        
    if((ppm == 0) || (ppm == 5000))
    {
#if DEBUG_TRACES_ENABLE        
        sprintf(&string[0],"> Skip value= %d\r\n",ppm);
        Serial.write(&string[0]);
#endif                
    }
    if(span_average_count < DISPLAY_MODULE_SPAN_AVERAGE)
    {
        span_average[span_average_count] = ppm;
        span_average_count++;
#if 0        
        sprintf(&string[0],"> Push value= %d, cnt= %d\r\n",ppm, span_average_count);
        Serial.write(&string[0]);
#endif        
    }
    else
    {
        display_data.historical.p_ppm_in->value = display_module_get_average(&span_average[0], DISPLAY_MODULE_SPAN_AVERAGE);
#if DEBUG_TRACES_ENABLE        
        sprintf(&string[0],"> Store value [%04d]= %d, len=%d\r\n",display_data.historical.p_ppm_in->_pos, display_data.historical.p_ppm_in->value, display_data.historical.ppm_length);
        Serial.write(&string[0]);
#endif         
        display_data.historical.p_ppm_in = display_data.historical.p_ppm_in->p_next;
        if(display_data.historical.ppm_length < DISPLAY_MODULE_HISTORICAL_SPAN)
        {
            display_data.historical.ppm_length++;
        }
        span_average_count = 0;       
    }
}

uint16_t display_module_get_average(uint16_t *data, uint16_t length)
{
    uint32_t sum = 0;
    uint16_t t = 0;

    for(t = 0; t < length; t++)
    {
        sum += data[t];
    }
    return (uint16_t)(sum/(uint32_t)length);
}

void display_module_draw_ppm()
{   
    sprintf(&display_string[0],"%04d",display_data.ppm);
    display.setFont(Roboto_Black_24);
    display.drawString(0,0, &display_string[0]);
    display.setFont(ArialMT_Plain_10);
    display.drawString(60,0,"ppm");    
}

void display_module_draw_ppm_graph()
{
    display.drawHorizontalLine(0, 61, 64);
    display.drawVerticalLine(0, 29, 32); 
    display.setFont(ArialMT_Plain_10);
    sprintf(&display_string[0],"%d", display_data.historical.ppm_max);
    display.drawString(65,24,&display_string[0]);
    sprintf(&display_string[0],"%d", display_data.historical.ppm_min);
    display.drawString(65,54,&display_string[0]);    
}

void display_module_draw_battery()
{    
    sprintf(&display_string[0],"%d", display_data.ppm);
    display.setFont(ArialMT_Plain_10);
    display.drawString(104,0,&display_string[0]);
    display.drawRect(102, 0, 22, 12);
    display.drawRect(124, 2, 4, 8);    
}

void display_module_set_warming(uint8_t state)
{
    display.drawRect(96, 48, 16, 16);
    display.display();    
}
void display_module_set_ble(uint8_t state)
{
    display.drawRect(112, 48, 16, 16);
    display.display();    
}

void display_module_set_reading(uint8_t state)
{
    display.drawRect(96, 32, 16, 16);
    display.display();    
}

void display_module_set_calibrating(uint8_t state)
{
    display.drawRect(112, 32, 16, 16);
    display.display();    
}
