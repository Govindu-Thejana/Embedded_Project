#include "lcd.h"
#include "i2c.h"
#include <util/delay.h>  // Added to resolve _delay_us and _delay_ms warnings

static void LCD_Send(uint8_t data) {
	I2C_Start();
	I2C_Write(LCD_ADDRESS << 1); // Write mode
	I2C_Write(data);
	I2C_Stop();
}

static void LCD_Write(uint8_t rs, uint8_t data) {
	uint8_t high_nibble = 0, low_nibble = 0;

	// Prepare high nibble (D4-D7) and control bits
	high_nibble = (rs << RS_PIN) | (0 << RW_PIN) | (1 << EN_PIN) | (1 << BL_PIN);
	if (data & 0x10) high_nibble |= (1 << D4_PIN);
	if (data & 0x20) high_nibble |= (1 << D5_PIN);
	if (data & 0x40) high_nibble |= (1 << D6_PIN);
	if (data & 0x80) high_nibble |= (1 << D7_PIN);

	// Prepare low nibble (D4-D7) and control bits
	low_nibble = (rs << RS_PIN) | (0 << RW_PIN) | (1 << EN_PIN) | (1 << BL_PIN);
	if (data & 0x01) low_nibble |= (1 << D4_PIN);
	if (data & 0x02) low_nibble |= (1 << D5_PIN);
	if (data & 0x04) low_nibble |= (1 << D6_PIN);
	if (data & 0x08) low_nibble |= (1 << D7_PIN);

	// Send high nibble with EN high, then low
	LCD_Send(high_nibble);
	_delay_us(1);
	LCD_Send(high_nibble & ~(1 << EN_PIN));
	_delay_us(50);

	// Send low nibble with EN high, then low
	LCD_Send(low_nibble);
	_delay_us(1);
	LCD_Send(low_nibble & ~(1 << EN_PIN));
	_delay_ms(2);
}

void LCD_Init(void) {
	_delay_ms(20); // Wait for LCD to power up
	LCD_Write(0, 0x02); // 4-bit mode
	LCD_Write(0, 0x28); // 4 lines, 5x8 font
	LCD_Write(0, 0x0C); // Display on, cursor off
	LCD_Write(0, 0x06); // Increment cursor
	LCD_Write(0, 0x01); // Clear display
	_delay_ms(2);
}

void LCD_Clear(void) {
	LCD_Write(0, 0x01); // Clear display
	_delay_ms(2);
}

void LCD_SetCursor(uint8_t col, uint8_t row) {
	uint8_t address;
	switch (row) {
		case 0: address = 0x00; break; // Line 1
		case 1: address = 0x40; break; // Line 2
		case 2: address = 0x14; break; // Line 3
		case 3: address = 0x54; break; // Line 4
		default: address = 0x00;
	}
	address += col;
	LCD_Write(0, 0x80 | address); // Set DDRAM address
}

void LCD_String(const char *str) {
	while (*str) {
		LCD_Write(1, *str++); // RS=1 for data
	}
}