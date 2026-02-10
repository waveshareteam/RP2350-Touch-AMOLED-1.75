/******************************************************************************
* | File       :   CST9217.c
* | Author     :   Waveshare Team / RP2350 Port by ChatGPT
* | Function   :   CST9217 Interface Functions (RP2350 pico-sdk)
* | Info       :
*----------------
* | This version:  V1.1
* | Date        :  2025-03-20
* | Info        :
#
# This software is provided "AS IS", without warranty of any kind.
******************************************************************************/
#include "CST9217.h"
#include "DEV_Config.h"

CST9217_Struct CST9217;

/******************************************************************************
function :  Send one byte of data to the specified register of CST9217
parameter:
******************************************************************************/
static void CST9217_I2C_Write_Byte(uint8_t reg, uint8_t value) {
    Wire1.beginTransmission(CST9217_I2C_ADDR);
    Wire1.write(reg);
    Wire1.write(value);
    Wire1.endTransmission();
}

/******************************************************************************
function :  Send nbytes to the specified register of CST9217
parameter:
******************************************************************************/
static void CST9217_I2C_Write_nByte(uint16_t reg, uint8_t *pData, uint32_t Len) {
    Wire1.beginTransmission(CST9217_I2C_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);
    Wire1.write(pData, Len);
    Wire1.endTransmission();
}


/******************************************************************************
function :  Read one byte of data from the specified register of CST9217
parameter:
******************************************************************************/
static uint8_t CST9217_I2C_Read_Byte(uint8_t reg) {
    uint8_t value;
  
    Wire1.beginTransmission(CST9217_I2C_ADDR);
    Wire1.write((byte)reg);
    Wire1.endTransmission();
  
    Wire1.requestFrom(CST9217_I2C_ADDR, (byte)1);
    value = Wire1.read();
  
    return value;
}

/******************************************************************************
function :  Read nbytes from CST9217
parameter:
******************************************************************************/
static void CST9217_I2C_Read_nByte(uint16_t reg, uint8_t *pData, uint32_t Len) {
    Wire1.beginTransmission(CST9217_I2C_ADDR);
    Wire1.write(reg >> 8);
    Wire1.write(reg & 0xFF);
    Wire1.endTransmission(false);  // repeated start

    Wire1.requestFrom(CST9217_I2C_ADDR, (uint8_t)Len);

    for (uint8_t i = 0; i < Len && Wire1.available(); i++) {
        pData[i] = Wire1.read();
    }
}

/******************************************************************************
function :  Reset the CST9217
parameter:
******************************************************************************/
void CST9217_Reset(void) {
    gpio_put(TOUCH_RST_PIN, 0);
    sleep_ms(10);
    gpio_put(TOUCH_RST_PIN, 1);
    sleep_ms(50);
}

/******************************************************************************
function :  Initialize the CST9217
parameter:
******************************************************************************/
void CST9217_Init(void) {
    // Hardware reset
    CST9217_Reset();

    sleep_ms(30);

    // Read and print configuration
    if(CST9217_Read_Config() == false) {
        Serial.printf("Failed to read CST9217 configuration.\n");
        return;
    }
    Serial.printf("CST9217 configuration read successfully.\n");
}

bool CST9217_Read_Config(void) {
    uint8_t data[4] = {0};
    uint8_t cmd_mode[2] = {0xD1, 0x01};

    CST9217_I2C_Write_nByte(CST9217_CMD_MODE_REG, cmd_mode, sizeof(cmd_mode));

    sleep_ms(10);
   
    CST9217_I2C_Read_nByte(CST9217_CHECKCODE_REG, data, 4);
       
    Serial.printf("Checkcode: 0x%02X%02X%02X%02X\n", data[0], data[1], data[2], data[3]);

    CST9217_I2C_Read_nByte(CST9217_RESOLUTION_REG, data, 4);
    uint16_t res_x = (data[1] << 8) | data[0];
    uint16_t res_y = (data[3] << 8) | data[2];
    Serial.printf("Resolution X: %d, Y: %d\n", res_x, res_y);

    CST9217_I2C_Read_nByte(CST9217_PROJECT_ID_REG, data, 4);
    uint16_t chipType = (data[3] << 8) | data[2];
    uint32_t projectID = (data[1] << 8) | data[0];
    Serial.printf("Chip Type: 0x%04X, ProjectID: 0x%04lX\n", chipType, projectID);
    return (chipType == CST9217_CHIP_ID);
}

bool CST9217_Read_Data(void) {
    uint8_t data[CST9217_DATA_LENGTH] = {0};

    CST9217_I2C_Read_nByte(CST9217_DATA_REG, data, sizeof(data));

    if (data[6] != CST9217_ACK_VALUE) {
        return false;
    }

    CST9217.points = data[5] & 0x7F;
    CST9217.points = (CST9217.points > CST9217_MAX_TOUCH_POINTS) ? CST9217_MAX_TOUCH_POINTS : CST9217.points;

    for (int i = 0; i < CST9217.points; i++) {
        uint8_t *p = &data[i * 5 + (i ? 2 : 0)];
        uint8_t status = p[0] & 0x0F;

        if (status == 0x06) {
            CST9217.data[i].x = ((p[1] << 4) | (p[3] >> 4));
            CST9217.data[i].y = ((p[2] << 4) | (p[3] & 0x0F));

            CST9217.data[i].x = 466 - CST9217.data[i].x;
            CST9217.data[i].y = 466 - CST9217.data[i].y;

            // Serial.printf("Point %d: X=%d, Y=%d\n", i, CST9217.data[i].x, CST9217.data[i].y);
        }
    }
    return true;
}
