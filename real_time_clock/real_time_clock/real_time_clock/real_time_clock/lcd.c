#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>

void LCD_Init() {
	_delay_ms(50);  // LCD Power-up time

	LCD_Command(0x33);  // Initialize in 4-bit mode
	LCD_Command(0x32);  // Set to 4-bit mode
	LCD_Command(0x28);  // Function Set: 4-bit, 2 lines, 5x7 dots
	LCD_Command(0x0C);  // Display ON, Cursor OFF
	LCD_Command(0x06);  // Entry mode: Auto Increment
	LCD_Command(0x01);  // Clear Display
	_delay_ms(2);
}


void LCD_Command(uint8_t cmd) {
	uint8_t upper_nibble = (cmd & 0xF0);  // Get upper nibble
	uint8_t lower_nibble = ((cmd << 4) & 0xF0);  // Get lower nibble

	I2C_Start();
	I2C_Write(LCD_ADDR << 1);
	I2C_Write(0x00);  // Command mode

	I2C_Write(upper_nibble | 0x04);  // Enable ON
	I2C_Write(upper_nibble);  // Enable OFF

	I2C_Write(lower_nibble | 0x04);  // Enable ON
	I2C_Write(lower_nibble);  // Enable OFF

	I2C_Stop();
	_delay_ms(2);
}


void LCD_Print(char *str) {
	I2C_Start();
	I2C_Write(LCD_ADDR << 1);
	I2C_Write(0x40);  // Data mode
	while (*str) {
		uint8_t upper_nibble = (*str & 0xF0);
		uint8_t lower_nibble = ((*str << 4) & 0xF0);

		I2C_Write(upper_nibble | 0x05);  // RS=1, Enable=1
		I2C_Write(upper_nibble | 0x01);  // RS=1, Enable=0

		I2C_Write(lower_nibble | 0x05);  // RS=1, Enable=1
		I2C_Write(lower_nibble | 0x01);  // RS=1, Enable=0

		str++;
	}
	I2C_Stop();
}


void LCD_SetCursor(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	LCD_Command(0x80 | (col + row_offsets[row]));
}

void LCD_Clear() {
	LCD_Command(0x01);  // Clear display command
	_delay_ms(2);       // LCD needs time to process
}
