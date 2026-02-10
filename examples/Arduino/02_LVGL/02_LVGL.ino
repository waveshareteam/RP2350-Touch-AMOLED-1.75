/*****************************************************************************
* | File      	:   LCD_1in69_Touch_test.c
* | Author      :   Waveshare team
* | Function    :   1.69inch LCD LVGL test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2025-04-22
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "LCD_Test.h"
#include "LVGL_example.h" 

int press_time = 0;

void setup()
{
    if (DEV_Module_Init() != 0)
        Serial.println("GPIO Init Fail!");
    else
        Serial.println("GPIO Init successful!");

    Serial.printf("AMOLED_1IN75_LCGL_test Demo\r\n");
    /*QSPI PIO Init*/
    QSPI_GPIO_Init(qspi);
    QSPI_PIO_Init(qspi);
    QSPI_4Wrie_Mode(&qspi);
    /*AMOLED Init*/
    AMOLED_1IN75_Init();
    AMOLED_1IN75_SetBrightness(60);
    AMOLED_1IN75_Clear(WHITE);
    /*Init touch screen*/ 
    CST9217_Init();
    /*Init RTC*/
    PCF85063A_Init();
    /*Init IMU*/
    QMI8658_init();
    /*Init LVGL*/
    LVGL_Init();
    Widgets_Init();
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}
