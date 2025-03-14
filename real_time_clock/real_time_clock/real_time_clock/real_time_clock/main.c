#include "i2c.h"
#include "lcd.h"
#include "rtc.h"
#include "keypad.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE_TIME 0
#define MODE_SELECTION 1
#define MODE_CALCULATOR 2
#define MODE_ALARM 3

uint8_t mode = MODE_TIME;
char expression[16];
uint8_t expr_index = 0;
uint8_t result_shown = 0;  // Flag to track if result is displayed

void Clear_Expression() {
	expr_index = 0;
	expression[0] = '\0';
	result_shown = 0;
	LCD_Clear();
	LCD_Print("Calculator Mode");
	LCD_SetCursor(0, 1);
}

int Evaluate_Expression(char *expr) {
	int num1 = 0, num2 = 0;
	char op = 0;
	sscanf(expr, "%d%c%d", &num1, &op, &num2);
	
	switch (op) {
		case '+': return num1 + num2;
		case '-': return num1 - num2;
		case '*': return num1 * num2;
		case '/':
		if (num2 != 0) return num1 / num2;
		else {
			LCD_SetCursor(0, 1);
			LCD_Print("Error: Div by 0");
			return 0;
		}
		default: return 0;
	}
}

void Calculator_Handler(char key) {
	if (key == 'C') {
		Clear_Expression();
	}
	else if (key == '=' && !result_shown) {
		int result = Evaluate_Expression(expression);
		char buffer[16];  // Reduced buffer size to match LCD width
		sprintf(buffer, "%s=%d", expression, result);
		LCD_SetCursor(0, 1);
		LCD_Print("                ");  // Clear line
		LCD_SetCursor(0, 1);
		LCD_Print(buffer);
		result_shown = 1;  // Mark result as shown
	}
	else if (key == '=' && result_shown) {
		// Second press of '=' clears screen and resets
		Clear_Expression();
	}
	else if (expr_index < 15 && !result_shown) {
		expression[expr_index++] = key;
		expression[expr_index] = '\0';
		LCD_SetCursor(0, 1);
		LCD_Print("                ");  // Clear line
		LCD_SetCursor(0, 1);
		LCD_Print(expression);
	}
}

void Mode_Handler() {
	char key = Keypad_GetKey();

	if (key == 'C') {
		if (mode == MODE_TIME) {
			mode = MODE_SELECTION;
			LCD_Clear();
			LCD_Print(" Select Mode:");
			LCD_SetCursor(0, 1);
			LCD_Print("1-Calc  2-Alarm");
			} else {
			mode = MODE_TIME;
			LCD_Clear();
			LCD_Print("Time & Date");
		}
		} else if (mode == MODE_SELECTION) {
		if (key == '1') {
			mode = MODE_CALCULATOR;
			Clear_Expression();
			} else if (key == '2') {
			mode = MODE_ALARM;
			LCD_Clear();
			LCD_Print("Alarm Mode");
		}
		} else if (mode == MODE_CALCULATOR && key) {
		Calculator_Handler(key);
	}
}

int main(void) {
	char buffer[10];

	I2C_Init();
	LCD_Init();
	RTC_Init();
	Keypad_Init();
	
	while (1) {
		Mode_Handler();
		
		if (mode == MODE_TIME) {
			RTC_Time time = RTC_GetTime();
			LCD_SetCursor(0, 0);
			sprintf(buffer, "Time: %02d:%02d:%02d", time.hour, time.min, time.sec);
			LCD_Print(buffer);
			
			LCD_SetCursor(0, 1);
			sprintf(buffer, "Date: %02d/%02d/%04d", time.day, time.month, time.year);
			LCD_Print(buffer);
			
			_delay_ms(1000);
		}
		// MODE_ALARM can be implemented here if needed
	}
	
	return 0;  // Never reached
}