/*
 * keypad.h
 *
 * Created: 3/13/2025 3:35:50 PM
 *  Author: NC
 */ 
#ifndef KEYPAD_H
#define KEYPAD_H

#include <avr/io.h>

// Define keypad connections
#define ROW_PORT PORTD
#define ROW_DDR  DDRD
#define ROW_PIN  PIND

#define COL_PORT PORTB
#define COL_DDR  DDRB
#define COL_PIN  PINB

void Keypad_Init(void);
char Keypad_GetKey(void);

#endif