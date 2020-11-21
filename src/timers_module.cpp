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
#include "timers_module.h"
static volatile uint8_t timer_flags = 0x00;
static hw_timer_t * timers[tid_LAST];
static portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void timers_module_init()
{
    timers[tid_500ms] = timerBegin(tid_500ms, 80, true);
    timerAttachInterrupt(timers[tid_500ms], &timers_module_500ms_callback, true);
    timerAlarmWrite(timers[tid_500ms], 500000, true);
    timerAlarmEnable(timers[tid_500ms]);

    timers[tid_1000ms] = timerBegin(tid_1000ms, 80, true);
    timerAttachInterrupt(timers[tid_1000ms], &timers_module_1000ms_callback, true);
    timerAlarmWrite(timers[tid_1000ms], 1000000, true);
    timerAlarmEnable(timers[tid_1000ms]);
}

void timers_module_500ms_callback()
{
    portENTER_CRITICAL_ISR(&timerMux);
    timer_flags |= (0x00000001 << tid_500ms);
    portEXIT_CRITICAL_ISR(&timerMux);
}

void timers_module_1000ms_callback()
{
    portENTER_CRITICAL_ISR(&timerMux);
    timer_flags |= (0x00000001 << tid_1000ms);
    portEXIT_CRITICAL_ISR(&timerMux);
}

uint8_t timers_module_test(uint8_t timer_id)
{
    uint8_t result = 0x00;
    portENTER_CRITICAL_ISR(&timerMux);
    if(timer_flags & (0x01 << timer_id))
    {
        timer_flags &= ~(0x01 << timer_id);
        result++;
    }
    portEXIT_CRITICAL_ISR(&timerMux);
    return result;
}

