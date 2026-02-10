#include "DEV_Config.h"
#include "AMOLED_1in75.h"
#include "qspi_pio.h"
#include "QMI8658.h"
#include "PCF85063A.h"
#include "CST9217.h"
#include "AXP2101.h"
#include "image.h"
#include "GUI_Paint.h"
#include "psram_tool.h"
#include "rp_pico_alloc.h"

uint16_t color[] = {RED , BLUE};
int flag=0;

void GPIO_INT_callback(uint gpio, uint32_t events);

void setup()
{
   if (DEV_Module_Init() != 0)
        Serial.println("GPIO Init Fail!");
    else
        Serial.println("GPIO Init successful!");

    sleep_ms(1000);

    /*QSPI PIO Init*/
    QSPI_GPIO_Init(qspi);
    QSPI_PIO_Init(qspi);
    QSPI_4Wrie_Mode(&qspi);

    /*AXP2101*/
    AXP2101_Write(XPOWERS_AXP2101_ICC_CHG_SET, 0x09); // Charge current 300mA
    Serial.printf("XPOWERS_AXP2101_ICC_CHG_SET: 0x%02X\n", AXP2101_Read(XPOWERS_AXP2101_ICC_CHG_SET));

    /*GPIO INT*/
    DEV_KEY_Config(SYS_OUT);
    DEV_IRQ_SET(SYS_OUT, GPIO_IRQ_EDGE_FALL, &GPIO_INT_callback);
    DEV_KEY_Config(TOUCH_INT_PIN);
    DEV_IRQ_SET(TOUCH_INT_PIN, GPIO_IRQ_EDGE_FALL, &GPIO_INT_callback);

    /*AMOLED Init*/
    Serial.printf("1.75inch AMOLED demo...\r\n");
    AMOLED_1IN75_Init();
    AMOLED_1IN75_SetBrightness(60);
    AMOLED_1IN75_Clear(WHITE);

    CST9217_Init();
    
    uint32_t Imagesize = AMOLED_1IN75_HEIGHT*AMOLED_1IN75_WIDTH*2;
    uint16_t *BlackImage;
    if((BlackImage = (uint16_t *)malloc(Imagesize)) == NULL) {
        Serial.printf("Failed to apply for black memory...\r\n");
        exit(0);
    }

    /*1.Create a new image cache named IMAGE_RGB and fill it with white*/
    Paint_NewImage((uint8_t *)BlackImage, AMOLED_1IN75.WIDTH, AMOLED_1IN75.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_SetRotate(ROTATE_0);
    Paint_Clear(WHITE);
    AMOLED_1IN75_Display(BlackImage);

    /* GUI */
    Serial.printf("drawing...\r\n");
    /*2.Drawing on the image*/
#if 1
    Paint_DrawPoint(149, 186, BLACK, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(149, 191, BLACK, DOT_PIXEL_3X3, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(149, 196, BLACK, DOT_PIXEL_4X4, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(149, 201, BLACK, DOT_PIXEL_5X5, DOT_FILL_RIGHTUP);
    Paint_DrawPoint(149, 207, BLACK, DOT_PIXEL_6X6, DOT_FILL_RIGHTUP);

    Paint_DrawLine(157, 185, 187, 215, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(157, 215, 187, 185, MAGENTA, DOT_PIXEL_2X2, LINE_STYLE_SOLID);

    Paint_DrawLine(227, 200, 257, 200, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(242, 185, 242, 215, CYAN, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawRectangle(157, 185, 187, 215, RED, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(192, 185, 222, 215, BLUE, DOT_PIXEL_2X2, DRAW_FILL_FULL);

    Paint_DrawCircle(242, 200, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(277, 200, 15, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawNum(202, 220, 9.87654321, &Font24, 5, WHITE, BLACK);
    Paint_DrawString_EN(148, 220, "ABC", &Font24, 0x000f, 0xfff0);
    Paint_DrawString_CN(148, 240, "欢迎使用", &Font24CN, WHITE, BLUE);
    Paint_DrawString_EN(148, 280, "WaveShare", &Font24, RED, WHITE);

    AMOLED_1IN75_Display(BlackImage);
    DEV_Delay_ms(1000);
#endif

#if 1
    /*3.Refresh the picture in RAM to LCD*/
    Paint_DrawImage(gImage_image,0,0,AMOLED_1IN75.WIDTH,AMOLED_1IN75.HEIGHT);
    AMOLED_1IN75_Display(BlackImage);
    DEV_Delay_ms(100);
#endif

#if 1
    /*4.Display six-axis sensor data*/
    float acc[3], gyro[3];
    unsigned int tim_count = 0;
    QMI8658_init();
    // CST9217_Init();

    Paint_Clear(WHITE);
    Paint_DrawRectangle(0, 0, 466, 155, 0XF410, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 152, 466, 307, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawRectangle(0, 304, 466, 466, 0XAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    
    Paint_DrawString_EN(110, 80, "Long Press Out", &Font24, BLACK, 0XF410);
    Paint_DrawString_EN(120, 168, "ACC_X = " , &Font24, BLACK, 0X4F30);
    Paint_DrawString_EN(120, 211, "ACC_Y = " , &Font24, BLACK, 0X4F30);
    Paint_DrawString_EN(120, 254, "ACC_Z = ", &Font24, BLACK, 0X4F30);
    Paint_DrawString_EN(120, 318, "GYR_X = ", &Font24, BLACK, 0XAD55);
    Paint_DrawString_EN(120, 361, "GYR_Y = ", &Font24, BLACK, 0XAD55);
    Paint_DrawString_EN(120, 404, "GYR_Z = ", &Font24, BLACK, 0XAD55);
    AMOLED_1IN75_Display(BlackImage);

    while (true)
    {
        QMI8658_read_xyz(acc, gyro, &tim_count);
        // Serial.printf("acc_x   = %4.3fmg , acc_y  = %4.3fmg , acc_z  = %4.3fmg\r\n", acc[0], acc[1], acc[2]);
        // Serial.printf("gyro_x  = %4.3fdps, gyro_y = %4.3fdps, gyro_z = %4.3fdps\r\n"s, gyro[0], gyro[1], gyro[2]);
        // Serial.printf("tim_count = %d\r\n", tim_count);
        
        Paint_DrawRectangle(255, 168, 450, 290, 0X4F30, DOT_PIXEL_2X2, DRAW_FILL_FULL);
        Paint_DrawRectangle(255, 318, 450, 466, 0XAD55, DOT_PIXEL_2X2, DRAW_FILL_FULL);
        Paint_DrawNum(255, 168, acc[0], &Font24, 2, BLACK , 0X4F30);
        Paint_DrawNum(255, 211, acc[1], &Font24, 2, BLACK , 0X4F30);
        Paint_DrawNum(255, 254, acc[2], &Font24, 2, BLACK, 0X4F30);
        Paint_DrawNum(255, 318, gyro[0], &Font24, 2, BLACK, 0XAD55);
        Paint_DrawNum(255, 361, gyro[1], &Font24, 2, BLACK, 0XAD55);
        Paint_DrawNum(255, 404, gyro[2], &Font24, 2, BLACK, 0XAD55);

        AMOLED_1IN75_Display(BlackImage);
        DEV_Delay_ms(100);
        if (flag == 1)
        {
            flag = 0;
            static int presss_time = 0;
            presss_time++;
            if(presss_time > 3)
                break;         
        }
    }
#endif

#if 1
    /*5.Drawing test*/
    Paint_Clear(WHITE);
    AMOLED_1IN75_Display(BlackImage);
    Paint_DrawRectangle(0, 0, 466, 60, 0X2595, DOT_PIXEL_2X2, DRAW_FILL_FULL);
    Paint_DrawString_EN(140, 30, "Touch test", &Font24, BLACK, 0X2595);
    AMOLED_1IN75_Display(BlackImage);
    while (true)
    {
        if (flag)
        {
            CST9217_Read_Data();
            for(uint8_t i = 0; i < CST9217.points; i++)
            {
                int x = CST9217.data[i].x;
                int y = CST9217.data[i].y;
                if(x + 8 > AMOLED_1IN75_WIDTH) x = AMOLED_1IN75_WIDTH - 8;
                if(y + 8 > AMOLED_1IN75_HEIGHT) y = AMOLED_1IN75_HEIGHT - 8;
                Paint_DrawPoint(x, y, color[i], DOT_PIXEL_8X8, DOT_FILL_AROUND);
                AMOLED_1IN75_DisplayWindows(x-2, y-2, x + 8, y + 8, BlackImage);
                Serial.printf("Point[%d]: X:%d Y:%d\r\n", i, x, y);
            }
            flag = 0;
        }
        __asm__ volatile("nop");
    }
#endif

     /* Module Exit */
     free(BlackImage);
     BlackImage = NULL;
     
     DEV_Module_Exit();
}

void loop()
{
     DEV_Delay_ms(1000);
} 

void GPIO_INT_callback(uint gpio, uint32_t events)
{
    if(gpio == SYS_OUT)
    {
        watchdog_reboot(0,0,0);
    }
    else if(gpio == TOUCH_INT_PIN)
    {
        flag = 1;
    }
}