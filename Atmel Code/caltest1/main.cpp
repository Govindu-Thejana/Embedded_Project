#define F_CPU 16000000UL // 16 MHz clock frequency for Arduino Uno
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

// LCD Pin Definitions
#define LCD_RS PB2    // Pin 10
#define LCD_RW PB1    // Pin 9
#define LCD_EN PB0    // Pin 8
#define LCD_D4 PC0    // Pin A0
#define LCD_D5 PC1    // Pin A1
#define LCD_D6 PC2    // Pin A2
#define LCD_D7 PC3    // Pin A3

// Keypad Pin Definitions
#define ROW1 PD0      // Pin 0
#define ROW2 PD1      // Pin 1
#define ROW3 PD2      // Pin 2
#define ROW4 PD3      // Pin 3
#define COL1 PD4      // Pin 4
#define COL2 PD5      // Pin 5
#define COL3 PD6      // Pin 6
#define COL4 PD7      // Pin 7

// Function Prototypes
void LCD_Command(unsigned char cmnd);
void LCD_Data(unsigned char data);
void LCD_Init(void);
void LCD_String(const char *str);
void LCD_Clear(void);
char Keypad_Read(void);
void Calculator(void);

int main(void) {
	// Set LCD pins as output
	DDRB |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_EN); // PB2, PB1, PB0 as output
	DDRC |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7); // PC0-PC3 as output

	// Set Keypad pins: Rows as input, Columns as output
	DDRD &= ~(1 << ROW1 | 1 << ROW2 | 1 << ROW3 | 1 << ROW4); // Rows as input
	DDRD |= (1 << COL1 | 1 << COL2 | 1 << COL3 | 1 << COL4);  // Columns as output
	PORTD |= (1 << ROW1 | 1 << ROW2 | 1 << ROW3 | 1 << ROW4); // Enable pull-ups for rows

	// Initialize LCD
	LCD_Init();
	LCD_String("Calculator Ready");

	// Main loop
	while (1) {
		Calculator();
	}
}

void LCD_Command(unsigned char cmnd) {
	// Clear data pins
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);

	// Send high nibble
	if (cmnd & 0x10) PORTC |= (1 << LCD_D4);
	if (cmnd & 0x20) PORTC |= (1 << LCD_D5);
	if (cmnd & 0x40) PORTC |= (1 << LCD_D6);
	if (cmnd & 0x80) PORTC |= (1 << LCD_D7);

	PORTB &= ~(1 << LCD_RS); // RS = 0 for command
	PORTB &= ~(1 << LCD_RW); // RW = 0 for write
	PORTB |= (1 << LCD_EN);  // EN = 1
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN); // EN = 0
	_delay_us(50);

	// Send low nibble
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
	if (cmnd & 0x01) PORTC |= (1 << LCD_D4);
	if (cmnd & 0x02) PORTC |= (1 << LCD_D5);
	if (cmnd & 0x04) PORTC |= (1 << LCD_D6);
	if (cmnd & 0x08) PORTC |= (1 << LCD_D7);

	PORTB |= (1 << LCD_EN);  // EN = 1
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN); // EN = 0
	_delay_ms(2);
}

void LCD_Data(unsigned char data) {
	// Clear data pins
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);

	// Send high nibble
	if (data & 0x10) PORTC |= (1 << LCD_D4);
	if (data & 0x20) PORTC |= (1 << LCD_D5);
	if (data & 0x40) PORTC |= (1 << LCD_D6);
	if (data & 0x80) PORTC |= (1 << LCD_D7);

	PORTB |= (1 << LCD_RS);  // RS = 1 for data
	PORTB &= ~(1 << LCD_RW); // RW = 0 for write
	PORTB |= (1 << LCD_EN);  // EN = 1
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN); // EN = 0
	_delay_us(50);

	// Send low nibble
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
	if (data & 0x01) PORTC |= (1 << LCD_D4);
	if (data & 0x02) PORTC |= (1 << LCD_D5);
	if (data & 0x04) PORTC |= (1 << LCD_D6);
	if (data & 0x08) PORTC |= (1 << LCD_D7);

	PORTB |= (1 << LCD_EN);  // EN = 1
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN); // EN = 0
	_delay_ms(2);
}

void LCD_Init(void) {
	_delay_ms(20);          // Wait for LCD to power up
	LCD_Command(0x02);      // 4-bit mode
	LCD_Command(0x28);      // 2 lines, 5x7 matrix
	LCD_Command(0x0C);      // Display on, cursor off
	LCD_Command(0x06);      // Increment cursor
	LCD_Command(0x01);      // Clear display
	_delay_ms(2);
}

void LCD_String(const char *str) {
	while (*str) {
		LCD_Data(*str++);
	}
}

void LCD_Clear(void) {
	LCD_Command(0x01);
	_delay_ms(2);
}

char Keypad_Read(void) {
	// Set all columns high
	PORTD |= (1 << COL1 | 1 << COL2 | 1 << COL3 | 1 << COL4);

	// Check Row 1
	PORTD &= ~(1 << COL1); // Set COL1 low
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '7'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '4'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '1'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return 'O'; } // ON/C
	PORTD |= (1 << COL1);

	// Check Row 2
	PORTD &= ~(1 << COL2); // Set COL2 low
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '8'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '5'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '2'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '0'; }
	PORTD |= (1 << COL2);

	// Check Row 3
	PORTD &= ~(1 << COL3); // Set COL3 low
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '9'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '6'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '3'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '='; }
	PORTD |= (1 << COL3);

	// Check Row 4
	PORTD &= ~(1 << COL4); // Set COL4 low
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '/'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return 'x'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '-'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '+'; }
	PORTD |= (1 << COL4);

	return 0; // No key pressed
}

void Calculator(void) {
	char key, num1[16], num2[16], op;
	int i = 0, j = 0;
	long result = 0;
	float result_float = 0.0;
	static char last_result[16] = ""; // Store last result to display persistently

	// If there's a previous result, display it
	if (last_result[0] != '\0') {
		LCD_Clear();
		LCD_String(last_result);
		// Wait for ON/C to start new calculation
		while ((key = Keypad_Read()) != 'O') {
			_delay_ms(10);
		}
		while (Keypad_Read() != 0) _delay_ms(10); // Wait for key release
	}

	// Start new calculation
	LCD_Clear();
	LCD_String("1st Num:");
	while ((key = Keypad_Read()) == 0) _delay_ms(10);
	i = 0;
	while (key != '+' && key != '-' && key != 'x' && key != '/' && key != 'O') {
		if (key >= '0' && key <= '9') {
			LCD_Data(key);
			num1[i++] = key;
		}
		while ((key = Keypad_Read()) != 0) _delay_ms(10);
		while ((key = Keypad_Read()) == 0) _delay_ms(10);
	}
	num1[i] = '\0';
	
	// If ON/C is pressed, reset
	if (key == 'O') {
		last_result[0] = '\0'; // Clear last result
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}
	
	op = key;
	LCD_Command(0xC0);
	LCD_String("2nd Num:");
	while ((key = Keypad_Read()) == 0) _delay_ms(10);
	j = 0;
	while (key != '=' && key != 'O') {
		if (key >= '0' && key <= '9') {
			LCD_Data(key);
			num2[j++] = key;
		}
		while ((key = Keypad_Read()) != 0) _delay_ms(10);
		while ((key = Keypad_Read()) == 0) _delay_ms(10);
	}
	num2[j] = '\0';

	// If ON/C is pressed, reset
	if (key == 'O') {
		last_result[0] = '\0';
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	// Convert strings to numbers
	long n1 = atol(num1);
	long n2 = atol(num2);

	// Perform calculation
	switch (op) {
		case '+': result = n1 + n2; break;
		case '-': result = n1 - n2; break;
		case 'x': result = n1 * n2; break;
		case '/':
		if (n2 != 0) result_float = (float)n1 / (float)n2;
		else {
			LCD_Clear();
			LCD_String("Error: Divide by 0");
			strcpy(last_result, "Error: Divide by 0");
			return;
		}
		break;
		default:
		LCD_Clear();
		LCD_String("Invalid Operator");
		strcpy(last_result, "Invalid Operator");
		return;
	}

	// Store and display result
	LCD_Clear();
	if (op == '/') {
		dtostrf(result_float, 5, 2, last_result);
		LCD_String(last_result);
		} else {
		ltoa(result, last_result, 10);
		LCD_String(last_result);
	}
}