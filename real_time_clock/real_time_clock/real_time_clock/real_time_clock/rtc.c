#include "rtc.h"
#include "i2c.h"

#define RTC_ADDR 0x51  // PCF8563 RTC I2C Address

void RTC_Init(void) {
	I2C_Start();
	I2C_Write(RTC_ADDR << 1);
	I2C_Write(0x00);
	I2C_Write(0x00);  // Start the RTC
	I2C_Stop();
}

RTC_Time RTC_GetTime(void) {
	RTC_Time time;

	I2C_Start();
	I2C_Write(RTC_ADDR << 1);
	I2C_Write(0x02);
	I2C_Stop();

	I2C_Start();
	I2C_Write((RTC_ADDR << 1) | 1);
	time.sec = I2C_ReadAck() & 0x7F;
	time.min = I2C_ReadAck() & 0x7F;
	time.hour = I2C_ReadAck() & 0x3F;
	time.day = I2C_ReadAck() & 0x3F;
	time.month = I2C_ReadAck() & 0x1F;
	time.year = 2000 + I2C_ReadNack();

	I2C_Stop();
	return time;
}
