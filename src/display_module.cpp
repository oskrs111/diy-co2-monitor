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
#include "config_module.h"  
#include "sensor_module.h"  
#include "display_module.h"  
#include "display_module_fonts.h"  
#include "display_module_icons.h"  
#include "SSD1306Wire.h"  
#if DEBUG_TRACES_ENABLE && DISPLAY_TRACES_ENABLE
    char debug_string[256];    
#endif  
static struct display_preferences* preferences = 0x00;
static SSD1306Wire display(0x3c, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
static struct st_display_data display_data;
static char display_string[64];
static uint16_t *span_average = 0x00;

void display_module_init()
{
    preferences = &config_module_get_preferences()->display;
    memset(&display_data, 0x00, sizeof(display_data));
    display_module_historical_init();
    display.init();
    display.setI2cAutoInit(true);
    display.flipScreenVertically();    
    display_module_draw_splash();
    display_module_set_battery(100);
    display_module_set_ipaddress((char*)"000.000");
    display_module_set_ppm(0);    
    display_module_set_warming(0);
    display_module_set_reading(0);
    display_module_set_calibrating(0);
    display_module_set_ble(0);
    Serial.write("> display_module_init() => Done!\r\n");
}

void display_module_historical_init()
{
    uint16_t t = 0;
    span_average = new uint16_t[preferences->average_interval];

    for(t = 1; t < (DISPLAY_MODULE_HISTORICAL_SPAN - 1); t++)
    {        
        display_data.historical.historical_values[t].p_next = &display_data.historical.historical_values[t+1];
        display_data.historical.historical_values[t].p_prev = &display_data.historical.historical_values[t-1];
        display_data.historical.historical_values[t]._pos = t;        
        display_data.historical.historical_values[t].value = 0x00;
    }

    display_data.historical.historical_values[0].p_next = &display_data.historical.historical_values[1];
    display_data.historical.historical_values[0].p_prev = &display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)];
    display_data.historical.historical_values[0]._pos = 0;
    display_data.historical.historical_values[0].value = 0x00;

    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)].p_next = &display_data.historical.historical_values[0];
    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)].p_prev = &display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 2)];
    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)]._pos = (DISPLAY_MODULE_HISTORICAL_SPAN - 1);
    display_data.historical.historical_values[(DISPLAY_MODULE_HISTORICAL_SPAN - 1)].value = 0x00;

    display_data.historical.p_ppm_in = &display_data.historical.historical_values[0];    
    display_data.historical.ppm_length = 0; 
    display_data.historical.ppm_max = SENSOR_MIN_PPM;
    display_data.historical.ppm_min = SENSOR_MAX_PPM;               
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

void display_module_set_temperature(float temp)
{
    display_data.temperature = temp;
}

void display_module_push_historical(uint16_t ppm)
{    
    static uint16_t span_average_count = 0;    
    struct st_ppm_value *p = 0x00;
    uint16_t t = 0x00;      
    if((ppm == SENSOR_MIN_PPM) || (ppm == SENSOR_MAX_PPM))
    {
#if DEBUG_TRACES_ENABLE && DISPLAY_TRACES_ENABLE        
        sprintf(&debug_string[0],"> Skip value= %d\r\n",ppm);
        Serial.write(&debug_string[0]);
        return;
#endif                
    }

    if(span_average_count < preferences->average_interval)
    {
        span_average[span_average_count] = ppm;
        span_average_count++;
#if DEBUG_TRACES_ENABLE && DISPLAY_TRACES_ENABLE
        sprintf(&debug_string[0],"> Push value= %d, cnt= %d\r\n",ppm, span_average_count);
        Serial.write(&debug_string[0]);
#endif        
    }
    else
    {
        display_data.historical.p_ppm_in->value = display_module_get_average(&span_average[0], preferences->average_interval);
#if DEBUG_TRACES_ENABLE && DISPLAY_TRACES_ENABLE        
        sprintf(&debug_string[0],"> Store value [%04d]= %d, len=%d\r\n",display_data.historical.p_ppm_in->_pos, display_data.historical.p_ppm_in->value, display_data.historical.ppm_length);
        Serial.write(&debug_string[0]);
#endif         
        display_data.historical.p_ppm_in = display_data.historical.p_ppm_in->p_next;
        if(display_data.historical.ppm_length < DISPLAY_MODULE_HISTORICAL_SPAN)
        {
            display_data.historical.ppm_length++;
        }
        span_average_count = 0;    

        display_data.historical.ppm_max = SENSOR_MIN_PPM;
        display_data.historical.ppm_min = SENSOR_MAX_PPM;
        p = display_data.historical.p_ppm_in->p_prev;
        for(t = 0; t < DISPLAY_MODULE_HISTORICAL_SPAN; t++)
        {
            if(p->value != 0)
            {
                display_module_max_min_update(p->value);    
            }
            p = p->p_prev;
        }
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

void display_module_set_battery(uint16_t battery)
{
    display_data.battery = battery;
}

void display_module_set_ipaddress(char* ipaddress)
{
    if(strlen(&ipaddress[0]) < sizeof(display_data.ipaddress))
    {
        memcpy(&display_data.ipaddress, &ipaddress[0], strlen(&ipaddress[0]));
        display_data.ipaddress[strlen(&ipaddress[0])] = 0;
    }    
    display_module_draw_ipaddress();
}

void display_module_draw_splash()
{
    display.clear();    
    display.drawXbm(23,0,logo_image_width,logo_image_height,&logo_image[0]);
    display.setFont(ArialMT_Plain_10);
    display.drawString(23,54,"diy-co2-monitor");   
    display.display();    
}

void display_module_draw_ppm()
{   
    char *format = (char*)"%04d";
    if(display_data.ppm < 1000)
    {
        format = (char*)" %03d";
    }        
    sprintf(&display_string[0],&format[0],display_data.ppm);    
    display.setFont(Roboto_Black_24);
    display.drawString(0,0, &display_string[0]);
    display.setFont(ArialMT_Plain_10);
    display.drawString(60,0,"ppm");    
}

void display_module_draw_ppm_graph()
{
    #define VERTICAL_START 38
    #define VERTICAL_HEIGHT 30    
    struct st_ppm_value *p = 0x00;
    uint16_t t = 0x00;
    uint16_t max = display_data.historical.ppm_max;
    uint16_t min = display_data.historical.ppm_min;
    uint8_t x_pos = 0x00;
    uint8_t y_pos = 0x00;
    uint8_t step_size = 0x00;
    uint8_t length = 0x00;

    if(display_data.historical.ppm_length == 0x00)
    {
        return;
    }

    if((max - min) < VERTICAL_HEIGHT)
    {
        max = (((max + min)/2)+VERTICAL_HEIGHT);
        min = (((max + min)/2)-VERTICAL_HEIGHT);
    }
    else
    {
        max += (VERTICAL_HEIGHT/4);
        min -= (VERTICAL_HEIGHT/4);
    }

    step_size = ((max - min) / VERTICAL_HEIGHT);
    step_size = (step_size == 0x00)?1:step_size;
    x_pos = display_data.historical.ppm_length;

    p = display_data.historical.p_ppm_in->p_prev;
    for(t = display_data.historical.ppm_length; t > 0; t--)
    {                
        length = ((p->value - min)/ step_size); 
        y_pos = (VERTICAL_START + (VERTICAL_HEIGHT - length));
        display.drawVerticalLine(x_pos, y_pos, length);     
#if 0
        sprintf(&debug_string[0],"> Draw pos=%d, value= %d, x= %d, y= %d, ln= %d, st= %d, max= %d, min= %d\r\n", 
        p->_pos, p->value, x_pos, y_pos, length, step_size, max, min );
        Serial.write(&debug_string[0]);
#endif       
        p = p->p_prev;
        x_pos--;
    }
    display.drawHorizontalLine(0, 63, 64);
    display.drawVerticalLine(0, VERTICAL_START, 32); 
    display.setFont(ArialMT_Plain_10);
    sprintf(&display_string[0],"%d", max);
    display.drawString(65,24,&display_string[0]);
    sprintf(&display_string[0],"%d", min);
    display.drawString(65,54,&display_string[0]);    
}

void display_module_max_min_update(uint16_t ppm)
{
    if(ppm > display_data.historical.ppm_max) 
    {
            display_data.historical.ppm_max = ppm;
    }

    if(ppm < display_data.historical.ppm_min) 
    {
        display_data.historical.ppm_min = ppm;
    }

#if DEBUG_TRACES_ENABLE && DISPLAY_TRACES_ENABLE
        sprintf(&debug_string[0],"> Update M/m ppm= %d, max= %d, min= %d\r\n", ppm, display_data.historical.ppm_max, display_data.historical.ppm_min);
        Serial.write(&debug_string[0]);
#endif 
}

void display_module_draw_battery()
{    
    sprintf(&display_string[0],"%d", display_data.ppm);
    display.setFont(ArialMT_Plain_10);
    display.drawString(104,0,&display_string[0]);
    display.drawRect(102, 0, 22, 12);
    display.drawRect(124, 2, 4, 8);    
}

void display_module_draw_ipaddress()
{        
    display.setFont(ArialMT_Plain_10);
    display.drawString(95,0,&display_data.ipaddress[0]);
    display.drawString(89,0,"<");
}

void display_module_set_warming(uint8_t state)
{
    if(state > 0 )
    {
        display.drawCircle(120, 40, 6);
    }    
    else
    {
        display.fillCircle(120, 40, 6);
    }  
}
void display_module_set_ble(uint8_t state)
{
    display.drawRect(112, 48, 16, 16);
}

void display_module_set_reading(uint8_t state)
{
    if(state > 0 )
    {
        display.drawCircle(120, 57, 6);
    }    
    else
    {
        display.fillCircle(120, 57, 6);
    }
}

void display_module_set_calibrating(uint8_t state)
{
    display.drawRect(112, 32, 16, 16);    
}


void display_module_defaults(struct display_preferences* preferences)
{
    preferences->average_interval = DISPLAY_MODULE_AVERAGE_INTERVAL_DEFAULT;
}

char* display_module_historical_2_json()
{
    static char buffer[((DISPLAY_MODULE_HISTORICAL_SPAN * 5) + 64)];
    struct st_ppm_value *p = 0x00;
    char* j = &buffer[0];
    memset(j, 0x00, sizeof(buffer));
    j += sprintf(j,"\"historical\":{\"span\":%d, \"interval\":%d, \"max\":%d, \"min\":%d, \"data\":[",         
        DISPLAY_MODULE_HISTORICAL_SPAN, 
        DISPLAY_MODULE_AVERAGE_INTERVAL_DEFAULT,
        display_data.historical.ppm_max,
        display_data.historical.ppm_min);
    p = display_data.historical.p_ppm_in->p_prev;
    for(uint16_t t = display_data.historical.ppm_length; t > 0; t--)
    {                
        j += sprintf(j,"%d,", p->value);
        p = p->p_prev;    
    }
    j--; /**< To remove the last ',' */    
    j += sprintf(j,"]}");
    return &buffer[0];
}