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
	DDRB |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_EN);
	DDRC |= (1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
	DDRD &= ~(1 << ROW1 | 1 << ROW2 | 1 << ROW3 | 1 << ROW4);
	DDRD |= (1 << COL1 | 1 << COL2 | 1 << COL3 | 1 << COL4);
	PORTD |= (1 << ROW1 | 1 << ROW2 | 1 << ROW3 | 1 << ROW4);

	LCD_Init();
	LCD_String("Calculator Ready");
	
	while (1) {
		Calculator();
	}
}

// [LCD_Command, LCD_Data, LCD_Init, LCD_String, LCD_Clear, Keypad_Read remain unchanged...]

void LCD_Command(unsigned char cmnd) {
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
	if (cmnd & 0x10) PORTC |= (1 << LCD_D4);
	if (cmnd & 0x20) PORTC |= (1 << LCD_D5);
	if (cmnd & 0x40) PORTC |= (1 << LCD_D6);
	if (cmnd & 0x80) PORTC |= (1 << LCD_D7);
	PORTB &= ~(1 << LCD_RS);
	PORTB &= ~(1 << LCD_RW);
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_us(50);
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
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
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
	if (data & 0x10) PORTC |= (1 << LCD_D4);
	if (data & 0x20) PORTC |= (1 << LCD_D5);
	if (data & 0x40) PORTC |= (1 << LCD_D6);
	if (data & 0x80) PORTC |= (1 << LCD_D7);
	PORTB |= (1 << LCD_RS);
	PORTB &= ~(1 << LCD_RW);
	PORTB |= (1 << LCD_EN);
	_delay_us(1);
	PORTB &= ~(1 << LCD_EN);
	_delay_us(50);
	PORTC &= ~(1 << LCD_D4 | 1 << LCD_D5 | 1 << LCD_D6 | 1 << LCD_D7);
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
	_delay_ms(20);
	LCD_Command(0x02);
	LCD_Command(0x28);
	LCD_Command(0x0C);
	LCD_Command(0x06);
	LCD_Command(0x01);
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
	PORTD |= (1 << COL1 | 1 << COL2 | 1 << COL3 | 1 << COL4);
	PORTD &= ~(1 << COL1);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '7'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '4'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '1'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return 'O'; }
	PORTD |= (1 << COL1);
	PORTD &= ~(1 << COL2);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '8'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '5'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '2'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '0'; }
	PORTD |= (1 << COL2);
	PORTD &= ~(1 << COL3);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '9'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return '6'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '3'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '='; }
	PORTD |= (1 << COL3);
	PORTD &= ~(1 << COL4);
	if (!(PIND & (1 << ROW1))) { _delay_ms(50); return '/'; }
	if (!(PIND & (1 << ROW2))) { _delay_ms(50); return 'x'; }
	if (!(PIND & (1 << ROW3))) { _delay_ms(50); return '-'; }
	if (!(PIND & (1 << ROW4))) { _delay_ms(50); return '+'; }
	PORTD |= (1 << COL4);
	return 0;
}

void Calculator(void) {
	char key, num1[6], num2[6], op = 0; // Limit each to 5 digits + null terminator
	int i = 0, j = 0, pos = 0;         // pos tracks cursor position
	long result = 0;
	float result_float = 0.0;
	static char last_result[16] = "";   // Persistent result storage

	// If there's a previous result, display it
	if (last_result[0] != '\0') {
		LCD_Clear();
		LCD_String(last_result);
		while ((key = Keypad_Read()) != 'O') {
			_delay_ms(10);
		}
		while (Keypad_Read() != 0) _delay_ms(10); // Wait for key release
	}

	// Start new calculation
	LCD_Clear();
	pos = 0;

	// Input first number (max 5 digits to fit 16-char line)
	while ((key = Keypad_Read()) == 0) _delay_ms(10);
	while (key != '+' && key != '-' && key != 'x' && key != '/' && key != 'O' && pos < 5) {
		if (key >= '0' && key <= '9') {
			LCD_Data(key);
			num1[i++] = key;
			pos++;
		}
		while ((key = Keypad_Read()) != 0) _delay_ms(10); // Debounce
		while ((key = Keypad_Read()) == 0) _delay_ms(10); // Wait for next key
	}
	num1[i] = '\0';

	// Check for reset
	if (key == 'O') {
		last_result[0] = '\0';
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	// Display and store operator
	if (pos < 15) {
		op = key;
		LCD_Data(op);
		pos++;
		} else {
		LCD_Clear();
		LCD_String("Input Too Long");
		_delay_ms(2000);
		return;
	}

	// Input second number (max 5 digits)
	while ((key = Keypad_Read()) == 0) _delay_ms(10);
	while (key != '=' && key != 'O' && pos < 10) { // Leave room for "=result"
		if (key >= '0' && key <= '9') {
			LCD_Data(key);
			num2[j++] = key;
			pos++;
		}
		while ((key = Keypad_Read()) != 0) _delay_ms(10); // Debounce
		while ((key = Keypad_Read()) == 0) _delay_ms(10); // Wait for next key
	}
	num2[j] = '\0';

	// Check for reset
	if (key == 'O') {
		last_result[0] = '\0';
		LCD_Clear();
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	// Perform calculation and display result when '=' is pressed
	if (key == '=' && pos < 15) {
		long n1 = atol(num1);
		long n2 = atol(num2);

		LCD_Data('='); // Append '=' without clearing
		pos++;

		switch (op) {
			case '+':
			result = n1 + n2;
			ltoa(result, last_result, 10);
			break;
			case '-':
			result = n1 - n2;
			ltoa(result, last_result, 10);
			break;
			case 'x':
			result = n1 * n2;
			ltoa(result, last_result, 10);
			break;
			case '/':
			if (n2 != 0) {
				result_float = (float)n1 / n2;
				dtostrf(result_float, 5, 1, last_result); // 5 chars, 1 decimal
				} else {
				strcpy(last_result, "Err");
			}
			break;
			default:
			strcpy(last_result, "OpErr");
			break;
		}

		// Append result if space allows
		if (pos + strlen(last_result) <= 16) {
			LCD_String(last_result);
			} else {
			// If result won't fit, overwrite with error
			LCD_Command(0x80); // Move cursor to start of line
			LCD_String("Result Too Long");
			strcpy(last_result, "Result Too Long");
			_delay_ms(2000);
			return;
		}

		// Update pos after appending result
		pos += strlen(last_result);
		} else if (pos >= 15) {
		LCD_Command(0x80); // Move cursor to start
		LCD_String("Input Too Long");
		strcpy(last_result, "Input Too Long");
		_delay_ms(2000);
		return;
	}
}