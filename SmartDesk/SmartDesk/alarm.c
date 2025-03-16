/*
 * alarm.c
 *
 * Created: 3/15/2025 1:01:09 PM
 *  Author: NC
 */ 
#include "alarm.h"
#include "rtc.h"
#include "lcd.h"
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t alarm_active = 0;  // 1 = Alarm is ringing

void Alarm_Init(void) {
	EICRA |= (1 << ISC01);  // Falling edge triggers INT0
	EIMSK |= (1 << INT0);   // Enable INT0 interrupt
	sei();  // Enable global interrupts
}

void Alarm_Set(uint8_t hour, uint8_t minute) {
	RTC_SetAlarm(hour, minute);
	LCD_Clear();
	LCD_Print("Alarm Set:");
	LCD_SetCursor(0, 1);
	char buffer[6];
	sprintf(buffer, "%02d:%02d", hour, minute);
	LCD_Print(buffer);
	_delay_ms(1000);  // Fixed: Show confirmation for 1 second
}

void Buzzer_Init(void) {
	DDRB |= (1 << PB4);  // Set PB4 as an output
}

void Buzzer_On(void) {
	PORTB |= (1 << PB4);  // Turn buzzer ON
	alarm_active = 1;  // Mark alarm as active
}

void Buzzer_Off(void) {
	PORTB &= ~(1 << PB4);  // Turn buzzer OFF
	alarm_active = 0;  // Mark alarm as inactive
}

// Interrupt Service Routine for INT0 (Alarm Interrupt)
ISR(INT0_vect) {
	Buzzer_On();
}