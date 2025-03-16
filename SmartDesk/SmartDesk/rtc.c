/*
 * rtc.c
 *
 * Created: 3/15/2025 11:49:28 AM
 * Author: NC
 */ 
#include "rtc.h"
#include "i2c.h"
#define RTC_ADDR 0x51  // PCF8563 RTC I2C Address

void RTC_Init(void) {
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x00);
    I2C_Write(0x00);  // Start the RTC (clear ST1/ST2, enable clock)
    I2C_Stop();
}

// Helper function for BCD to Decimal conversion
uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Helper function for Decimal to BCD conversion
uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

RTC_Time RTC_GetTime(void) {
    RTC_Time time;

    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x02); // Start reading from seconds register
    I2C_Stop();

    I2C_Start();
    I2C_Write((RTC_ADDR << 1) | 1);
    time.sec   = bcd_to_decimal(I2C_ReadAck() & 0x7F);  // 0x02: Seconds
    time.min   = bcd_to_decimal(I2C_ReadAck() & 0x7F);  // 0x03: Minutes
    time.hour  = bcd_to_decimal(I2C_ReadAck() & 0x3F);  // 0x04: Hours
    time.day   = bcd_to_decimal(I2C_ReadAck() & 0x3F);  // 0x05: Days
    I2C_ReadAck();  // 0x06: Weekdays (skip, not used)
    time.month = bcd_to_decimal(I2C_ReadAck() & 0x1F);  // 0x07: Months
    time.year  = bcd_to_decimal(I2C_ReadNack()) + 2000; // 0x08: Years
    I2C_Stop();

    return time;
}

void RTC_SetAlarm(uint8_t hour, uint8_t minute) {
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x09);  // Minute alarm register
    I2C_Write(decimal_to_bcd(minute) & 0x7F);  // 0x09: Minute alarm
    I2C_Write(decimal_to_bcd(hour) & 0x3F);    // 0x0A: Hour alarm
    I2C_Write(0x80);  // 0x0B: Day alarm disabled
    I2C_Write(0x80);  // 0x0C: Weekday alarm disabled
    I2C_Stop();
    
    // Clear any existing alarm flag before enabling
    RTC_ClearAlarm();
    
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x01);  // Control/status 2
    I2C_Write(0x02);  // Enable AIE (Alarm Interrupt Enable)
    I2C_Stop();
}

// Check if alarm has triggered - MISSING FUNCTION
uint8_t RTC_CheckAlarm(void) {
    uint8_t status;
    
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x01);  // Control/Status 2 register
    I2C_Stop();
    
    I2C_Start();
    I2C_Write((RTC_ADDR << 1) | 1);
    status = I2C_ReadNack();
    I2C_Stop();
    
    return (status & 0x08) ? 1 : 0;  // Check AF (Alarm Flag) bit (bit 3)
}

// Clear the alarm flag - MISSING FUNCTION
void RTC_ClearAlarm(void) {
    uint8_t status;
    
    // First read current register value
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x01);  // Control/Status 2 register
    I2C_Stop();
    
    I2C_Start();
    I2C_Write((RTC_ADDR << 1) | 1);
    status = I2C_ReadNack();
    I2C_Stop();
    
    // Clear the AF bit while preserving other bits
    status &= ~0x08;  // Clear AF bit (bit 3)
    
    // Write back the modified value
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x01);  // Control/Status 2 register
    I2C_Write(status);
    I2C_Stop();
}

void RTC_SetTime(RTC_Time *time) {
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x02);  // Start from seconds register
    I2C_Write(decimal_to_bcd(time->sec) & 0x7F);    // 0x02: Seconds
    I2C_Write(decimal_to_bcd(time->min) & 0x7F);    // 0x03: Minutes
    I2C_Write(decimal_to_bcd(time->hour) & 0x3F);   // 0x04: Hours
    I2C_Write(decimal_to_bcd(time->day) & 0x3F);    // 0x05: Days
    I2C_Write(0x00);  // 0x06: Weekdays (set to 0, not used)
    I2C_Write(decimal_to_bcd(time->month) & 0x1F);  // 0x07: Months
    I2C_Write(decimal_to_bcd(time->year - 2000));   // 0x08: Years
    I2C_Stop();
}

// Function to read back alarm time for verification
void RTC_GetAlarm(uint8_t *hour, uint8_t *minute) {
    I2C_Start();
    I2C_Write(RTC_ADDR << 1);
    I2C_Write(0x09);  // Minute alarm register
    I2C_Stop();
    
    I2C_Start();
    I2C_Write((RTC_ADDR << 1) | 1);
    *minute = bcd_to_decimal(I2C_ReadAck() & 0x7F);
    *hour = bcd_to_decimal(I2C_ReadNack() & 0x3F);
    I2C_Stop();
}