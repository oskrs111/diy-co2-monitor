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
#include "module_common.h"
#include "sensor_module.h"
#include "config_module.h"
#include "MHZ19.h"
static SoftwareSerial serial(SENSOR_RX_PIN, SENSOR_TX_PIN);                   
static uint16_t warming = SENSOR_WARMING_TIME;
static struct sensor_preferences* preferences = 0x00;
static MHZ19 sensor;
static uint16_t ppm = 0;
static float temp = 0;
void sensor_module_init()
{  
    preferences = &config_module_get_preferences()->sensor;
    serial.begin(SENSOR_BAUDRATE);                                    
    sensor.begin(serial);                                
    if(preferences > 0x00)
    {
        sensor.autoCalibration((preferences->flags & SENSOR_FLAGS_AUTOCALIBRATION_ENABLE)?1:0);  
    }    

    sensor.printCommunication(false, true); /**< TEMPORAL */
    Serial.write("> sensor_module_init() => Done!\r\n");
}

void sensor_module_defaults(struct sensor_preferences* preferences)
{
    preferences->flags = SENSOR_FLAGS_DEFAULT;        
}

uint16_t sensor_module_ppm_get()
{
    return ppm;
}

float sensor_module_temperature_get()
{
    return temp;
}

uint16_t sensor_module_ppm_read()
{   
    static uint16_t ppm_last = 0x00; 
    uint16_t ppm_attempt = 0x00;
    if( warming > 0x00 ) warming--;
    ppm_attempt = sensor.getCO2(true, true);    
    
    if((ppm_last == 0x00) && (ppm_attempt > 0x00))
    {
        ppm_last = ppm_attempt;
    }    

    if((ppm_attempt > 0x00) && (abs(ppm_last - ppm_attempt) < SENSOR_PMM_DIFF_MAX))
    /**< Sometimes 'sensor.getCO2(...)' returns zero or very high values*/
    {
        ppm = ppm_attempt;
    }    
#if DEBUG_TRACES_ENABLE && SENSOR_TRACES_ENABLE    
    Serial.print("> sensor_module_ppm_read() => ");
    Serial.println(ppm);
#endif    

    sensor_module_check_health();
    return ppm;
}

float sensor_module_temperature_read()
{           
    static uint16_t temp_last = 0x00; 
    temp = sensor.getTemperature(false, true);    
    if(temp_last == 0x00)
    {
        temp_last = temp;
    }    

    if(abs(temp_last - temp) > SENSOR_TEMP_DIFF_MAX)
    {
        temp = temp_last;
    }
    else
    {
        temp_last = temp;
    }

#if DEBUG_TRACES_ENABLE && SENSOR_TRACES_ENABLE    
    Serial.print("> sensor_module_temp_read() => ");
    Serial.println(temp);
#endif    
    sensor_module_check_health();
    return temp;
}

uint8_t sensor_module_warming()
{
   static uint8_t w = 0x00;
   w =  (warming > 0)?0x01:0x00;
   return w;
}

void sensor_module_check_health()
{
    static uint8_t error_counter = 0x00;
    switch(sensor.errorCode)
    {        	    
	    case RESULT_TIMEOUT:
            error_counter++;
	    case RESULT_MATCH:
	    case RESULT_CRC:
	    case RESULT_FILTER:            
            Serial.print("> sensor_module_check_health(errorCode, count) => ");
            Serial.print(sensor.errorCode);
            Serial.print(", ");
            Serial.println(error_counter);
        break;

        case RESULT_OK:
        default:
        break;
    }

    if(error_counter > SENSOR_MAX_ERROR)
    {
        sensor.recoveryReset();
        error_counter = 0x00;
        Serial.println("> sensor_module_check_health() => reset");
    }
}