#ifndef RTC_H
#define RTC_H

#include <avr/io.h>

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} RTC_Time;

void RTC_Init(void);
RTC_Time RTC_GetTime(void);

#endif
