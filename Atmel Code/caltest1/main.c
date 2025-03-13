
#define F_CPU 16000000UL // 16 MHz clock frequency
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "keypad.h" // Include the keypad header

// LCD Pin Definitions (Updated to avoid keypad conflict)
#define LCD_RS PC4    // Pin A4
#define LCD_RW PC5    // Pin A5
#define LCD_EN PB5    // Pin 13
#define LCD_D4 PC0    // Pin A0
#define LCD_D5 PC1    // Pin A1
#define LCD_D6 PC2    // Pin A2
#define LCD_D7 PC3    // Pin A3

// Function Prototypes
void LCD_Command(unsigned char cmnd);
void LCD_Data(unsigned char data);
void LCD_Init(void);
void LCD_String(const char *str);
void LCD_Clear(void);
void Calculator(void);

int main(void) {
	// Configure LCD pins as outputs
	DDRC |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	DDRB |= (1 << LCD_EN);

	Keypad_Init(); // Initialize keypad from keypad.h
	LCD_Init();
	LCD_String("Calculator Ready");
	_delay_ms(1000);

	while (1) {
		Calculator();
	}
}

void LCD_Command(unsigned char cmnd) {
	PORTC &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
	if (cmnd & 0x10) PORTC |= (1 << LCD_D4);
	if (cmnd & 0x20) PORTC |= (1 << LCD_D5);
	if (cmnd & 0x40) PORTC |= (1 << LCD_D6);
	if (cmnd & 0x80) PORTC |= (1 << LCD_D7);
	PORTC &= ~(1 << LCD_RS); // Command mode
	PORTC &= ~(1 << LCD_RW); // Write mode
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_us(50);

	PORTC &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
	if (cmnd & 0x01) PORTC |= (1 << LCD_D4);
	if (cmnd & 0x02) PORTC |= (1 << LCD_D5);
	if (cmnd & 0x04) PORTC |= (1 << LCD_D6);
	if (cmnd & 0x08) PORTC |= (1 << LCD_D7);
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_ms(2);
}

void LCD_Data(unsigned char data) {
	PORTC &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
	if (data & 0x10) PORTC |= (1 << LCD_D4);
	if (data & 0x20) PORTC |= (1 << LCD_D5);
	if (data & 0x40) PORTC |= (1 << LCD_D6);
	if (data & 0x80) PORTC |= (1 << LCD_D7);
	PORTC |= (1 << LCD_RS);  // Data mode
	PORTC &= ~(1 << LCD_RW); // Write mode
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_us(50);

	PORTC &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));
	if (data & 0x01) PORTC |= (1 << LCD_D4);
	if (data & 0x02) PORTC |= (1 << LCD_D5);
	if (data & 0x04) PORTC |= (1 << LCD_D6);
	if (data & 0x08) PORTC |= (1 << LCD_D7);
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_ms(2);
}

void LCD_Init(void) {
	_delay_ms(20);       // Wait for LCD to power up
	LCD_Command(0x02);   // 4-bit mode
	LCD_Command(0x28);   // 2 lines, 5x8 font
	LCD_Command(0x0C);   // Display on, cursor off
	LCD_Command(0x06);   // Increment cursor
	LCD_Command(0x01);   // Clear display
	_delay_ms(2);
}

void LCD_String(const char *str) {
	while (*str) {
		LCD_Data(*str++);
	}
}

void LCD_Clear(void) {
	LCD_Command(0x01); // Clear display
	_delay_ms(2);
}

void Calculator(void) {
	char key, num1[6] = {0}, num2[6] = {0}, op = 0;
	int i = 0, j = 0;
	long result = 0;
	float result_float = 0.0;
	char result_str[16] = {0};

	LCD_Clear();

	// Get first number
	while (1) {
		key = Keypad_GetKey(); // Use keypad.h function
		if (key && (key >= '0' && key <= '9') && i < 5) {
			num1[i++] = key;
			LCD_Data(key);
			} else if (key == '+' || key == '-' || key == '*' || key == '/' || key == 'C') {
			break;
		}
		_delay_ms(10);
	}
	num1[i] = '\0';

	if (key == 'C') {
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	op = key;
	LCD_Data(op);

	// Get second number
	while (1) {
		key = Keypad_GetKey(); // Use keypad.h function
		if (key && (key >= '0' && key <= '9') && j < 5) {
			num2[j++] = key;
			LCD_Data(key);
			} else if (key == '=') {
			break;
			} else if (key == 'C') {
			LCD_Clear();
			LCD_String("Calculator Ready");
			_delay_ms(1000);
			return;
		}
		_delay_ms(10);
	}
	num2[j] = '\0';

	long n1 = atol(num1);
	long n2 = atol(num2);
	LCD_Data('=');

	// Calculate result
	switch (op) {
		case '+': result = n1 + n2; ltoa(result, result_str, 10); break;
		case '-': result = n1 - n2; ltoa(result, result_str, 10); break;
		case '*': result = n1 * n2; ltoa(result, result_str, 10); break;
		case '/':
		if (n2 != 0) {
			result_float = (float)n1 / n2;
			dtostrf(result_float, 6, 2, result_str);
			} else {
			strcpy(result_str, "Error: Div by 0");
		}
		break;
		default: strcpy(result_str, "Error: Invalid Op"); break;
	}

	LCD_String(result_str);

	// Wait for 'C' to reset
	while (1) {
		key = Keypad_GetKey(); // Use keypad.h function
		if (key == 'C') {
			LCD_Clear();
			LCD_String("Calculator Ready");
			_delay_ms(1000);
			break;
		}
		_delay_ms(10);
	}
}