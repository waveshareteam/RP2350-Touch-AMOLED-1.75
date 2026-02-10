#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT       i2c1
#define I2C_SDA        6
#define I2C_SCL        7
#define I2C_FREQ       100000

#define DEVICE_ADDRESS     0x50
#define DEVICE_ADDRESS_R   0x54

uint8_t readData[4] = {0};

void i2c_master_init() {
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// -------------------- I2C WRITE --------------------
bool i2c_write(uint8_t addr, const uint8_t *data, size_t len) {
    int ret = i2c_write_blocking(I2C_PORT, addr, data, len, false);
    return (ret >= 0);
}

// -------------------- I2C READ --------------------
bool i2c_read(uint8_t addr, uint8_t *data, size_t len) {
    int ret = i2c_read_blocking(I2C_PORT, addr, data, len, false);
    return (ret >= 0);
}

// -------------------- Initwrite --------------------
void initwrite() {
    uint8_t data[] = {0x08, 0x00, 0x51, 0xAA, 0x04, 0x00, 0x00, 0x00};

    if (!i2c_write(DEVICE_ADDRESS, data, sizeof(data))) {
        printf("Write failed\n");
        return;
    }
    sleep_ms(100);

    if (!i2c_read(DEVICE_ADDRESS_R, readData, sizeof(readData))) {
        printf("Read failed\n");
        return;
    }
}

// -------------------- READ NMEA DATA --------------------
void writeDataToI2C() {
    initwrite();

    uint32_t dataLength = 
        (readData[0]) |
        (readData[1] << 8) |
        (readData[2] << 16) |
        (readData[3] << 24);

    if (dataLength == 0) return;

    uint8_t data2[] = {0x00, 0x20, 0x51, 0xAA};
    uint8_t totalData[sizeof(data2) + sizeof(readData)];

    memcpy(totalData, data2, sizeof(data2));
    memcpy(totalData + sizeof(data2), readData, sizeof(readData));

    sleep_ms(100);

    if (!i2c_write(DEVICE_ADDRESS, totalData, sizeof(totalData))) {
        printf("Write2 failed\n");
        return;
    }

    uint8_t *dynamicReadData = (uint8_t *)malloc(dataLength);
    if (!dynamicReadData) {
        printf("Malloc failed\n");
        return;
    }

    sleep_ms(100);

    if (!i2c_read(DEVICE_ADDRESS_R, dynamicReadData, dataLength)) {
        printf("Dynamic read failed\n");
        free(dynamicReadData);
        return;
    }

    // OUTPUT NMEA
    for (uint32_t i = 0; i < dataLength; i++) {
        putchar((char)dynamicReadData[i]);
    }
    printf("\n");

    free(dynamicReadData);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    printf("LC76G I2C Start...\n");

    i2c_master_init();

    while (1) {
        writeDataToI2C();
        sleep_ms(500);
    }
}
