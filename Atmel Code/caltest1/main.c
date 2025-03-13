#define F_CPU 16000000UL // 16 MHz clock frequency

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "keypad.h"
#include "lcd.h"

// Buzzer pin
#define BUZZER_PIN PD0    // Pin D0

// Function Prototypes
void Calculator(void);

int main(void) {
	// Configure buzzer pin as output
	DDRD |= (1 << BUZZER_PIN);

	LCD_Init();
	LCD_SetCursor(0, 0);
	LCD_String("Calculator Ready");
	_delay_ms(1000);

	Keypad_Init(); // Initialize keypad

	while (1) {
		Calculator();
	}
}

void Calculator(void) {
	char key, num1[6] = {0}, num2[6] = {0}, op = 0;
	int i = 0, j = 0;
	long result = 0;
	float result_float = 0.0;
	char result_str[16] = {0};

	LCD_Clear();

	// Display labels on each line
	LCD_SetCursor(0, 0);
	LCD_String("Num1: ");
	LCD_SetCursor(0, 1);
	LCD_String("Op  : ");
	LCD_SetCursor(0, 2);
	LCD_String("Num2: ");
	LCD_SetCursor(0, 3);
	LCD_String("Res : ");

	// Get first number
	LCD_SetCursor(6, 0); // After "Num1: "
	while (1) {
		key = Keypad_GetKey();
		if (key && (key >= '0' && key <= '9') && i < 5) {
			num1[i++] = key;
			char temp[2] = {key, '\0'}; // Convert char to string
			LCD_SetCursor(6 + i - 1, 0); // Update cursor position
			LCD_String(temp);
			} else if (key == '+' || key == '-' || key == '*' || key == '/' || key == 'C') {
			break;
		}
		_delay_ms(10);
	}
	num1[i] = '\0';

	if (key == 'C') {
		LCD_Clear();
		LCD_SetCursor(0, 0);
		LCD_String("Calculator Ready");
		_delay_ms(1000);
		return;
	}

	op = key;
	char op_str[2] = {op, '\0'};
	LCD_SetCursor(6, 1); // After "Op  : "
	LCD_String(op_str);

	// Get second number
	LCD_SetCursor(6, 2); // After "Num2: "
	while (1) {
		key = Keypad_GetKey();
		if (key && (key >= '0' && key <= '9') && j < 5) {
			num2[j++] = key;
			char temp[2] = {key, '\0'};
			LCD_SetCursor(6 + j - 1, 2); // Update cursor position
			LCD_String(temp);
			} else if (key == '=') {
			break;
			} else if (key == 'C') {
			LCD_Clear();
			LCD_SetCursor(0, 0);
			LCD_String("Calculator Ready");
			_delay_ms(1000);
			return;
		}
		_delay_ms(10);
	}
	num2[j] = '\0';

	long n1 = atol(num1);
	long n2 = atol(num2);

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

	// Display result
	LCD_SetCursor(6, 3); // After "Res : "
	LCD_String(result_str);

	// Wait for 'C' to reset
	while (1) {
		key = Keypad_GetKey();
		if (key == 'C') {
			LCD_Clear();
			LCD_SetCursor(0, 0);
			LCD_String("Calculator Ready");
			_delay_ms(1000);
			break;
		}
		_delay_ms(10);
	}
}