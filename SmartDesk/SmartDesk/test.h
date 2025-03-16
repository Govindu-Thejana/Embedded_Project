/*
 * test.h
 *
 * Created: 3/15/2025 11:22:47 PM
 *  Author: NC
 */ 


#ifndef TEST_H_
#define TEST_H_
/*
/*
 * SmartDesk.c
 *
 * Created: 3/15/2025 11:32:55 AM
 * Author : NC
 */ 

#include "i2c.h"
#include "lcd.h"
#include "rtc.h"
#include "keypad.h"
#include "alarm.h"
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
uint8_t cursor_position = 0;  // Tracks position in HH:MM input
uint8_t alarm_hour = 0, alarm_minute = 0;
extern volatile uint8_t alarm_active;  // Defined in alarm.c
uint8_t cursor_blink_state = 0;  // 0 = visible, 1 = hidden
volatile uint8_t timer_count = 0;  // For cursor blinking

char expression[16];
uint8_t expr_index = 0;
uint8_t result_shown = 0;  // Flag to track if result is displayed

// Function to display current time and date from RTC
void Display_Time_And_Date(void) {
    RTC_Time current_time = RTC_GetTime();
    
    char time_buffer[17];
    char date_buffer[17];
    
    sprintf(time_buffer, "Time: %02d:%02d:%02d", 
            current_time.hour, current_time.min, current_time.sec);
    sprintf(date_buffer, "Date: %02d/%02d/%04d", 
            current_time.day, current_time.month, current_time.year);
    
    LCD_Clear();
    LCD_Print(time_buffer);
    LCD_SetCursor(0, 1);
    LCD_Print(date_buffer);
}

void Display_Alarm_Setting(void) {
    LCD_Clear();
    LCD_Print("Set Alarm HH:MM");
    LCD_SetCursor(0, 1);
    
    char buffer[16];
    sprintf(buffer, "%02d:%02d", alarm_hour, alarm_minute);
    LCD_Print(buffer);
    LCD_SetCursor(cursor_position, 1);
}

// Blink the cursor at the current position
void Blink_Cursor(void) {
    RTC_Time current_time = RTC_GetTime();
    
    if (mode == MODE_ALARM) {
        // Toggle cursor visibility every 500ms
        if (current_time.sec % 2 == 0 && cursor_blink_state == 0) {
            // Hide cursor/digit
            LCD_SetCursor(cursor_position, 1);
            LCD_Print(" ");
            cursor_blink_state = 1;
        } else if (current_time.sec % 2 == 1 && cursor_blink_state == 1) {
            // Show cursor/digit
            LCD_SetCursor(cursor_position, 1);
            if (cursor_position == 0)
                LCD_Print('0' + (alarm_hour / 10));
            else if (cursor_position == 1)
                LCD_Print('0' + (alarm_hour % 10));
            else if (cursor_position == 3)
                LCD_Print('0' + (alarm_minute / 10));
            else if (cursor_position == 4)
                LCD_Print('0' + (alarm_minute % 10));
            cursor_blink_state = 0;
        }
    }
}

void Alarm_Input_Handler(char key) {
    if (key >= '0' && key <= '9') {
        uint8_t digit = key - '0';
        
        if (cursor_position == 0) {  // First digit of hour
            if (digit <= 2) {  
                alarm_hour = (alarm_hour % 10) + (digit * 10);
            }
        } else if (cursor_position == 1) {  // Second digit of hour
            if ((alarm_hour / 10 == 2 && digit <= 3) || (alarm_hour / 10 < 2)) {
                alarm_hour = (alarm_hour / 10) * 10 + digit;
            }
        } else if (cursor_position == 3) {  // First digit of minute
            if (digit <= 5) {
                alarm_minute = (alarm_minute % 10) + (digit * 10);
            }
        } else if (cursor_position == 4) {  // Second digit of minute
            alarm_minute = (alarm_minute / 10) * 10 + digit;
        }
        
        Display_Alarm_Setting();
    } else if (key == '-') {  // Move cursor left
        if (cursor_position > 0) {
            if (cursor_position == 3) cursor_position = 1; // Skip ':'
            else cursor_position--;
            LCD_SetCursor(cursor_position, 1);
            cursor_blink_state = 0;  // Reset blink state
        }
    } else if (key == '+') {  // Move cursor right
        if (cursor_position < 4) {
            if (cursor_position == 1) cursor_position = 3; // Skip ':'
            else cursor_position++;
            LCD_SetCursor(cursor_position, 1);
            cursor_blink_state = 0;  // Reset blink state
        }
    } else if (key == '=') {
        if (alarm_active) {  
            Buzzer_Off();  // Turn off alarm if it's ringing
        } else {
            Alarm_Set(alarm_hour, alarm_minute);
            _delay_ms(2000);  // Show "Alarm Set" message for 2 seconds
            mode = MODE_TIME;  // Return to Time Mode after setting alarm
            Display_Time_And_Date();  // Show the time again
        }
    }
}

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
    } else if (key == '=' && !result_shown) {
        int result = Evaluate_Expression(expression);
        char buffer[16];  // Reduced buffer size to match LCD width
        sprintf(buffer, "%s=%d", expression, result);
        LCD_SetCursor(0, 1);
        LCD_Print("                ");  // Clear line
        LCD_SetCursor(0, 1);
        LCD_Print(buffer);
        result_shown = 1;  // Mark result as shown
    } else if (key == '=' && result_shown) {
        // Second press of '=' clears screen and resets
        Clear_Expression();
    } else if (expr_index < 15 && !result_shown) {
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
            LCD_Print("Select Mode:");
            LCD_SetCursor(0, 1);
            LCD_Print("1-Calc  2-Alarm");
        } else {
            mode = MODE_TIME;
            Display_Time_And_Date();
        }
    } else if (mode == MODE_SELECTION) {
        if (key == '1') {
            mode = MODE_CALCULATOR;
            Clear_Expression();
        } else if (key == '2') {
            mode = MODE_ALARM;
            alarm_hour = 0;
            alarm_minute = 0;
            cursor_position = 0;
            cursor_blink_state = 0;
            Display_Alarm_Setting();
        }
    } else if (mode == MODE_CALCULATOR && key) {
        Calculator_Handler(key);
    } else if (mode == MODE_ALARM) {
        Alarm_Input_Handler(key);
    }
}

void Update_Time_Display(void) {
    static uint8_t last_second = 0;
    RTC_Time current_time = RTC_GetTime();
    
    // Update time display once per second
    if (mode == MODE_TIME && current_time.sec != last_second) {
        Display_Time_And_Date();
        last_second = current_time.sec;
    }
}

int main(void) {
    I2C_Init();
    LCD_Init();
    RTC_Init();
    Keypad_Init();
    Alarm_Init();
    Buzzer_Init();
    
    // Display time and date on startup
    Display_Time_And_Date();

    while (1) {
        Mode_Handler();
        Update_Time_Display();
        
        if (mode == MODE_ALARM) {
            Blink_Cursor();
        }
        
        _delay_ms(50);  // Small delay to prevent excessive polling
    }
    return 0;
}


*/
#endif /* TEST_H_ */