#ifndef RTC_H_
#define RTC_H_

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
void RTC_SetTime(RTC_Time *time);
void RTC_SetAlarm(uint8_t hour, uint8_t minute);
uint8_t RTC_CheckAlarm(void);  // New function
void RTC_ClearAlarm(void);     // New function
void RTC_GetAlarm(uint8_t *hour, uint8_t *minute);  // New function

#endif /* RTC_H_ */