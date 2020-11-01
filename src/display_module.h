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
#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H
#define DISPLAY_MODULE_HISTORICAL_SPAN 60   /**< 60 minutes */
#define DISPLAY_MODULE_SPAN_AVERAGE 60      /**< 60 seconds */
#define DISPLAY_SDA_PIN 5
#define DISPLAY_SCL_PIN 4

struct st_ppm_value {
    struct st_ppm_value *p_prev;
    struct st_ppm_value *p_next;
    uint16_t value;
    uint16_t _pos;
};

struct st_ppm_historical 
{
    struct st_ppm_value historical_values[DISPLAY_MODULE_HISTORICAL_SPAN];    
    struct st_ppm_value *p_ppm_in;
    uint16_t ppm_max;
    uint16_t ppm_min;
    uint8_t ppm_length;        
    uint8_t _align[3];
};

struct st_display_data
{
    struct st_ppm_historical historical;
    uint32_t icon_warming : 1,
             icon_ble : 1,
             icon_calibrating: 1,
             icon_reading: 1;

    uint16_t ppm;
    uint16_t battery;
};

void display_module_init();
void display_module_historical_init();
void display_module_push_historical(uint16_t ppm);
uint16_t display_module_get_average(uint16_t *data, uint16_t length);
void display_module_clear();
void display_module_update();
void display_module_set_ppm(uint16_t ppm);
void display_module_set_battery(uint16_t battery);
void display_module_set_warming(uint8_t state);
void display_module_set_ble(uint8_t state);
void display_module_set_calibrating(uint8_t state);
void display_module_set_reading(uint8_t state);

void display_module_draw_ppm();
void display_module_draw_ppm_graph();
void display_module_draw_battery();
#endif