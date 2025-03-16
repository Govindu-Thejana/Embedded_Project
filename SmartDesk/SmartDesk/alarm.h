
/*
 * alarm.h
 *
 * Created: 3/15/2025 1:01:09 PM
 *  Author: NC
 */ 
#ifndef ALARM_H
#define ALARM_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void Alarm_Init(void);  // Initializes the external interrupt on INT0
void Alarm_Set(uint8_t hour, uint8_t minute);  // Sets the alarm time on PCF8563
void Buzzer_Init(void);  // Configures PB4 as output for the buzzer
void Buzzer_On(void);  // Turns the buzzer ON
void Buzzer_Off(void);  // Turns the buzzer OFF

#endif // ALARM_H
