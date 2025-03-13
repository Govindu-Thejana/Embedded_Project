/*
 * keypad.c
 *
 * Created: 3/13/2025 3:36:46 PM
 *  Author: NC
 */ 
#include "keypad.h"
#include <util/delay.h>

const char keymap[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

void Keypad_Init() {
	// Configure Rows (PD4 - PD7) as outputs, initially HIGH
	ROW_DDR |= 0xF0;  // PD4-PD7 as outputs
	ROW_PORT |= 0xF0; // Set to HIGH

	// Configure Columns (PB0 - PB3) as inputs with pull-ups
	COL_DDR &= ~0x0F;  // PB0-PB3 as inputs
	COL_PORT |= 0x0F;  // Enable pull-ups
}

char Keypad_GetKey() {
	for (uint8_t row = 0; row < 4; row++) {
		ROW_PORT &= ~(1 << (row + 4));  // Set row LOW
		_delay_ms(5);

		for (uint8_t col = 0; col < 4; col++) {
			if (!(COL_PIN & (1 << col))) {  // If column is LOW, key is pressed
				while (!(COL_PIN & (1 << col)));  // Wait until key is released
				ROW_PORT |= (1 << (row + 4));  // Reset row HIGH
				return keymap[row][col];  // Return key value
			}
		}
		ROW_PORT |= (1 << (row + 4));  // Reset row HIGH
	}
	return 0;  // No key pressed
}