#define F_CPU 16000000UL // 16 MHz clock frequency
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
	// Configure LCD pins as outputs
	DDRB |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_EN);
	DDRC |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	
	// Configure Keypad: Rows as inputs, Columns as outputs
	DDRD &= ~((1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4)); // Inputs
	DDRD |= (1 << COL1) | (1 << COL2) | (1 << COL3) | (1 << COL4);    // Outputs
	PORTD |= (1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4);   // Enable pull-ups on rows

	LCD_Init();
	LCD_String("Calculator Ready");
	_delay_ms(50);

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
	PORTB &= ~(1 << LCD_RS); // Command mode
	PORTB &= ~(1 << LCD_RW); // Write mode
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
	PORTB |= (1 << LCD_RS);  // Data mode
	PORTB &= ~(1 << LCD_RW); // Write mode
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

char Keypad_Read(void) {
	PORTD |= (1 << COL1) | (1 << COL2) | (1 << COL3) | (1 << COL4); // All columns high
	
	// Check Column 1
	PORTD &= ~(1 << COL1);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '7'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '4'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '1'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return 'C'; } // Clear/ON
	
	// Check Column 2
	PORTD |= (1 << COL1);
	PORTD &= ~(1 << COL2);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '8'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '5'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '2'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '0'; }
	
	// Check Column 3
	PORTD |= (1 << COL2);
	PORTD &= ~(1 << COL3);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '9'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '6'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '3'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '='; }
	
	// Check Column 4
	PORTD |= (1 << COL3);
	PORTD &= ~(1 << COL4);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '/'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '*'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '-'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '+'; }
	
	PORTD |= (1 << COL4); // Reset columns
	return 0;             // No key pressed
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
		key = Keypad_Read();
		if (key && (key >= '0' && key <= '9') && i < 5) {
			num1[i++] = key;
			LCD_Data(key);
			while (Keypad_Read()) _delay_ms(10); // Debounce
			} else if (key == '+' || key == '-' || key == '*' || key == '/' || key == 'C') {
			break;
		}
		_delay_ms(10);
	}
	num1[i] = '\0';

	if (key == 'C') { // Reset
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	// Store operator
	op = key;
	LCD_Data(op);

	// Get second number
	while (1) {
		key = Keypad_Read();
		if (key && (key >= '0' && key <= '9') && j < 5) {
			num2[j++] = key;
			LCD_Data(key);
			while (Keypad_Read()) _delay_ms(10); // Debounce
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

	// Calculate result
	long n1 = atol(num1);
	long n2 = atol(num2);
	LCD_Data('=');

	switch (op) {
		case '+':
		result = n1 + n2;
		ltoa(result, result_str, 10);
		break;
		case '-':
		result = n1 - n2;
		ltoa(result, result_str, 10);
		break;
		case '*':
		result = n1 * n2;
		ltoa(result, result_str, 10);
		break;
		case '/':
		if (n2 != 0) {
			result_float = (float)n1 / n2;
			dtostrf(result_float, 6, 2, result_str); // 6 chars, 2 decimals
			} else {
			strcpy(result_str, "Error: Div by 0");
		}
		break;
		default:
		strcpy(result_str, "Error: Invalid Op");
		break;
	}

	// Display result and wait for 'C'
	LCD_String(result_str);

	// Wait until 'C' is pressed to clear and reset
	while (1) {
		key = Keypad_Read();
		if (key == 'C') {
			while (Keypad_Read()) _delay_ms(10); // Debounce
			LCD_Clear();
			LCD_String("Calculator Ready");
			_delay_ms(1000);
			break;
		}
		_delay_ms(10);
	}
}