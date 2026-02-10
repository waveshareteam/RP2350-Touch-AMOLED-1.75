#include "AXP2101.h"
#include "XPowersParams.h"

uint8_t statusRegister[XPOWERS_AXP2101_INTSTS_CNT];
uint8_t intRegister[XPOWERS_AXP2101_INTSTS_CNT];
void AXP2101_Write(uint8_t reg, uint8_t val) 
{
    DEV_I2C_Write_Byte(AXP2101_SLAVE_ADDRESS, reg, val);
}

uint8_t AXP2101_Read(uint8_t reg) 
{
    uint8_t val;
    val = DEV_I2C_Read_Byte(AXP2101_SLAVE_ADDRESS, reg);
    return val;
}

void AXP2101_ClrIrqStatus()
{
    for (int i = 0; i < XPOWERS_AXP2101_INTSTS_CNT; i++) {
        AXP2101_Write(XPOWERS_AXP2101_INTSTS1 + i, 0xFF);
        statusRegister[i] = 0;
    }
}

uint32_t AXP2101_GetIrqStatus(void)
{
    statusRegister[0] = AXP2101_Read(XPOWERS_AXP2101_INTSTS1);
    statusRegister[1] = AXP2101_Read(XPOWERS_AXP2101_INTSTS2);
    statusRegister[2] = AXP2101_Read(XPOWERS_AXP2101_INTSTS3);
    return (uint32_t)(statusRegister[0] << 16) | (uint32_t)(statusRegister[1] << 8) | (uint32_t)(statusRegister[2]);
}

bool AXP2101_SetRegisterBit(uint8_t registers, uint8_t bit)
{
    int val = AXP2101_Read(registers);
    if (val == -1) {
        return false;
    }
    AXP2101_Write(registers, (val | (_BV(bit))));
    return true;
}

bool AXP2101_ClrRegisterBit(uint8_t registers, uint8_t bit)
{
    int val = AXP2101_Read(registers);
    if (val == -1) {
        return false;
    }
    AXP2101_Write(registers, (val & (~_BV(bit))));
    return true;
}

bool AXP2101_GetRegisterBit(uint8_t registers, uint8_t bit)
{
    int val = AXP2101_Read(registers);
    if (val == -1) {
        return false;
    }
    return val & _BV(bit);
}

uint16_t AXP2101_ReadRegisterH6L8(uint8_t highReg, uint8_t lowReg)
{
    int h6 = AXP2101_Read(highReg);
    int l8 = AXP2101_Read(lowReg);
    if (h6 == -1 || l8 == -1)return 0;
    return ((h6 & 0x3F) << 8) | l8;
}

uint16_t AXP2101_ReadRegisterH5L8(uint8_t highReg, uint8_t lowReg)
{
    int h5 = AXP2101_Read(highReg);
    int l8 = AXP2101_Read(lowReg);
    if (h5 == -1 || l8 == -1)return 0;
    return ((h5 & 0x1F) << 8) | l8;
}

void AXP2101_ADC_Config() 
{
    AXP2101_Write(XPOWERS_AXP2101_DC_VOL0_CTRL, 0x83); 
    sleep_ms(100);
}

void AXP2101_IRQ_Init() 
{
    AXP2101_Write(XPOWERS_AXP2101_INTEN1, 0x20); 
}

void AXP2101_SetInterruptImpl(uint32_t opts, bool Enable)
{
    uint8_t data = 0, value = 0;
    Serial.printf("%s - HEX:0x%x \n", Enable ? "Enable" : "DISABLE", opts);
    if (opts & 0x0000FF) {
        value = opts & 0xFF;
        Serial.printf("Write INT0: %x\n", value);
        data = AXP2101_Read(XPOWERS_AXP2101_INTEN1);
        intRegister[0] =  Enable ? (data | value) : (data & (~value));
        AXP2101_Write(XPOWERS_AXP2101_INTEN1, intRegister[0]);
    }
    if (opts & 0x00FF00) {
        value = opts >> 8;
        Serial.printf("Write INT1: %x\n", value);
        data = AXP2101_Read(XPOWERS_AXP2101_INTEN2);
        intRegister[1] =  Enable ? (data | value) : (data & (~value));
        AXP2101_Write(XPOWERS_AXP2101_INTEN2, intRegister[1]);
    }
    if (opts & 0xFF0000) {
        value = opts >> 16;
        Serial.printf("Write INT2: %x\n", value);
        data = AXP2101_Read(XPOWERS_AXP2101_INTEN3);
        intRegister[2] =  Enable ? (data | value) : (data & (~value));
        AXP2101_Write(XPOWERS_AXP2101_INTEN3, intRegister[2]);
    }
}

void AXP2101_DisableTSPinMeasure(void)
{
    AXP2101_ClrRegisterBit(XPOWERS_AXP2101_ADC_CHANNEL_CTRL, 1);
}

bool AXP2101_SetChargeTargetVoltage(uint8_t opt)
{
    if (opt >= XPOWERS_AXP2101_CHG_VOL_MAX)return false;
    int val = AXP2101_Read(XPOWERS_AXP2101_CV_CHG_VOL_SET);
    if (val == -1)return false;
    val &= 0xF8;
    AXP2101_Write(XPOWERS_AXP2101_CV_CHG_VOL_SET, val | opt);
    return true;
}

bool AXP2101_EnableTemperatureMeasure(void)
{
    return AXP2101_SetRegisterBit(XPOWERS_AXP2101_ADC_CHANNEL_CTRL, 4);
}

bool AXP2101_EnableBattDetection(void)
{
    return AXP2101_SetRegisterBit(XPOWERS_AXP2101_BAT_DET_CTRL, 0);
}

bool AXP2101_EnableVbusVoltageMeasure(void)
{
    return AXP2101_SetRegisterBit(XPOWERS_AXP2101_ADC_CHANNEL_CTRL, 2);
}

bool AXP2101_EnableBattVoltageMeasure(void)
{
    return AXP2101_SetRegisterBit(XPOWERS_AXP2101_ADC_CHANNEL_CTRL, 0);
}

bool AXP2101_EnableSystemVoltageMeasure(void)
{
    return AXP2101_SetRegisterBit(XPOWERS_AXP2101_ADC_CHANNEL_CTRL, 3);
}

void AXP2101_adcOn() 
{
    AXP2101_EnableTemperatureMeasure();
    AXP2101_EnableBattDetection();
    AXP2101_EnableVbusVoltageMeasure();
    AXP2101_EnableBattVoltageMeasure();
    AXP2101_EnableSystemVoltageMeasure();
}

bool AXP2101_IsCharging(void)
{
    return (AXP2101_Read(XPOWERS_AXP2101_STATUS2) >> 5) == 0x01;
}

bool AXP2101_IsDischarge(void)
{
    return (AXP2101_Read(XPOWERS_AXP2101_STATUS2) >> 5) == 0x02;
}

bool AXP2101_IsStandby(void)
{
    return (AXP2101_Read(XPOWERS_AXP2101_STATUS2) >> 5) == 0x00;
}

bool AXP2101_IsVbusGood(void)
{
    return AXP2101_GetRegisterBit(XPOWERS_AXP2101_STATUS1, 5);
}
bool AXP2101_IsVbusIn(void)
{
    return AXP2101_GetRegisterBit(XPOWERS_AXP2101_STATUS2, 3) == 0 && AXP2101_IsVbusGood();
}

bool AXP2101_IsBatteryConnect(void)
{
    return AXP2101_GetRegisterBit(XPOWERS_AXP2101_STATUS1, 3);
}

float AXP2101_ReadBattery() 
{
    uint16_t val = (AXP2101_Read(XPOWERS_AXP2101_ADC_DATA_RELUST0) << 4) | (AXP2101_Read(XPOWERS_AXP2101_ADC_DATA_RELUST1) & 0x0F);
    return val * 1.7; 
}

float AXP2101_ReadTemperature(void)
{
    uint16_t raw = AXP2101_ReadRegisterH6L8(XPOWERS_AXP2101_ADC_DATA_RELUST8, XPOWERS_AXP2101_ADC_DATA_RELUST9);
    return XPOWERS_AXP2101_CONVERSION(raw);
}

uint16_t AXP2101_GetVbusVoltage(void)
{
    if (!AXP2101_IsVbusIn()) {
        return 0;
    }
    return AXP2101_ReadRegisterH6L8(XPOWERS_AXP2101_ADC_DATA_RELUST4, XPOWERS_AXP2101_ADC_DATA_RELUST5);
}

uint16_t AXP2101_GetSystemVoltage(void)
{
    return AXP2101_ReadRegisterH6L8(XPOWERS_AXP2101_ADC_DATA_RELUST6, XPOWERS_AXP2101_ADC_DATA_RELUST7);
}

uint16_t AXP2101_GetBattVoltage(void)
{
    if (!AXP2101_IsBatteryConnect()) {
        return 0;
    }
    return AXP2101_ReadRegisterH5L8(XPOWERS_AXP2101_ADC_DATA_RELUST0, XPOWERS_AXP2101_ADC_DATA_RELUST1);
}

uint16_t AXP2101_GetBatteryPercent(void)
{
    if (!AXP2101_IsBatteryConnect()) {
        return 0;
    }
    return AXP2101_Read(XPOWERS_AXP2101_BAT_PERCENT_DATA);
}

xpowers_chg_status_t AXP2101_GetChargerStatus(void)
{
    int val = AXP2101_Read(XPOWERS_AXP2101_STATUS2);
    if (val == -1)return XPOWERS_AXP2101_CHG_STOP_STATE;
    val &= 0x07;
    return (xpowers_chg_status_t)val;
}

uint8_t AXP2101_ReadID()
{
    return AXP2101_Read(XPOWERS_AXP2101_IC_TYPE);
}

void AXP2101_Init()
{
    if(AXP2101_ReadID() != XPOWERS_AXP2101_CHIP_ID){
        Serial.printf("AXP2101 not found\n");
        return;
    }
    AXP2101_DisableTSPinMeasure();
    AXP2101_SetInterruptImpl(XPOWERS_AXP2101_ALL_IRQ,false);
    AXP2101_SetChargeTargetVoltage(3);
    AXP2101_ClrIrqStatus();
    AXP2101_SetInterruptImpl(XPOWERS_AXP2101_PKEY_SHORT_IRQ,true);
    AXP2101_adcOn();
}

void AXP2101_ReadAll(axp2101_t* axp2101)
{
    axp2101->powerTemprature = AXP2101_ReadTemperature();
    axp2101->isCharging = AXP2101_IsCharging();
    axp2101->isDischarge = AXP2101_IsDischarge();
    axp2101->isStandby = AXP2101_IsStandby();
    axp2101->isVbusGood = AXP2101_IsVbusGood();
    axp2101->isVbusIn = AXP2101_IsVbusIn();
    axp2101->batVoltage = AXP2101_GetBattVoltage();
    axp2101->vbusVoltage = AXP2101_GetVbusVoltage();
    axp2101->systemVoltage = AXP2101_GetSystemVoltage();
    axp2101->batPercent = AXP2101_GetBatteryPercent();
    axp2101->chargerStatus = AXP2101_GetChargerStatus();
}
