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
#include <avr/interrupt.h>

#define MODE_TIME 0
#define MODE_SELECTION 1
#define MODE_CALCULATOR 2
#define MODE_ALARM 3
#define MODE_SET_TIME 4  // New mode for setting date and time

uint8_t mode = MODE_TIME;
uint8_t cursor_position = 0;  // Tracks position in HH:MM input
uint8_t alarm_hour = 0, alarm_minute = 0;
extern volatile uint8_t alarm_active;  // Defined in alarm.c
uint8_t cursor_blink_state = 0;  // 0 = visible, 1 = hidden
volatile uint8_t timer_count = 0;  // For cursor blinking

// New variables for set date/time mode
RTC_Time new_time;
uint8_t datetime_setting_field = 0;  // 0-5: hour, min, sec, day, month, year

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

// New function to display the set date/time screen
void Display_DateTime_Setting(void) {
    LCD_Clear();
    
    char buffer[17];
    if (datetime_setting_field < 3) {
        // Setting time (hour, min, sec)
        sprintf(buffer, "Set Time:");
        LCD_Print(buffer);
        
        sprintf(buffer, "%02d:%02d:%02d", new_time.hour, new_time.min, new_time.sec);
        LCD_SetCursor(0, 1);
        LCD_Print(buffer);
    } else {
        // Setting date (day, month, year)
        sprintf(buffer, "Set Date:");
        LCD_Print(buffer);
        
        sprintf(buffer, "%02d/%02d/%04d", new_time.day, new_time.month, new_time.year);
        LCD_SetCursor(0, 1);
        LCD_Print(buffer);
    }
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
    } else if (mode == MODE_SET_TIME) {
        // Toggle cursor visibility for date/time setting
        if (current_time.sec % 2 == 0 && cursor_blink_state == 0) {
            // Hide cursor/digit
            uint8_t pos = 0;
            
            // Calculate position based on current field
            if (datetime_setting_field == 0) pos = 0;      // Hours tens
            else if (datetime_setting_field == 1) pos = 3; // Minutes tens
            else if (datetime_setting_field == 2) pos = 6; // Seconds tens
            else if (datetime_setting_field == 3) pos = 0; // Day tens
            else if (datetime_setting_field == 4) pos = 3; // Month tens
            else if (datetime_setting_field == 5) pos = 6; // Year thousands
            
            LCD_SetCursor(pos, 1);
            LCD_Print("  ");  // Hide two digits
            cursor_blink_state = 1;
        } else if (current_time.sec % 2 == 1 && cursor_blink_state == 1) {
            // Show cursor/digits
            Display_DateTime_Setting();
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

// New function to handle setting date and time
void DateTime_Input_Handler(char key) {
    if (key == '=') {
        // Save the new date and time
        RTC_SetTime(&new_time);
        LCD_Clear();
        LCD_Print("Date/Time Set!");
        _delay_ms(2000);
        mode = MODE_TIME;
        Display_Time_And_Date();
        return;
    } else if (key == '+') {
        // Move to next field
        datetime_setting_field = (datetime_setting_field + 1) % 6;
        Display_DateTime_Setting();
        return;
    } else if (key == '-') {
        // Move to previous field
        if (datetime_setting_field == 0) datetime_setting_field = 5;
        else datetime_setting_field--;
        Display_DateTime_Setting();
        return;
    }
    
    // Handle digit input
    if (key >= '0' && key <= '9') {
        uint8_t digit = key - '0';
        
        switch (datetime_setting_field) {
            case 0:  // Hour
                if (digit <= 2) {
                    new_time.hour = digit * 10;
                    new_time.hour += (new_time.hour % 10);
                    if (new_time.hour > 23) new_time.hour = 23;
                } else {
                    new_time.hour = (new_time.hour / 10) * 10 + digit;
                    if (new_time.hour > 23) new_time.hour = 23;
                }
                break;
                
            case 1:  // Minute
                if (digit <= 5) {
                    new_time.min = digit * 10;
                    new_time.min += (new_time.min % 10);
                    if (new_time.min > 59) new_time.min = 59;
                } else {
                    new_time.min = (new_time.min / 10) * 10 + digit;
                    if (new_time.min > 59) new_time.min = 59;
                }
                break;
                
            case 2:  // Second
                if (digit <= 5) {
                    new_time.sec = digit * 10;
                    new_time.sec += (new_time.sec % 10);
                    if (new_time.sec > 59) new_time.sec = 59;
                } else {
                    new_time.sec = (new_time.sec / 10) * 10 + digit;
                    if (new_time.sec > 59) new_time.sec = 59;
                }
                break;
                
            case 3:  // Day
                if (digit <= 3) {
                    new_time.day = digit * 10;
                    new_time.day += (new_time.day % 10);
                    if (new_time.day > 31) new_time.day = 31;
                } else {
                    new_time.day = (new_time.day / 10) * 10 + digit;
                    if (new_time.day > 31) new_time.day = 31;
                }
                break;
                
            case 4:  // Month
                if (digit <= 1) {
                    new_time.month = digit * 10;
                    new_time.month += (new_time.month % 10);
                    if (new_time.month > 12) new_time.month = 12;
                } else {
                    new_time.month = (new_time.month / 10) * 10 + digit;
                    if (new_time.month > 12) new_time.month = 12;
                }
                break;
                
            case 5:  // Year
                new_time.year = digit * 1000 + (new_time.year % 1000);
                if (new_time.year < 2000) new_time.year = 2000;
                if (new_time.year > 2099) new_time.year = 2099;
                break;
        }
        
        Display_DateTime_Setting();
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
			_delay_ms(2000);
			LCD_Clear();
  
            LCD_Print("1-Calc  2-Alarm");
            LCD_SetCursor(0, 1);
            LCD_Print("3-Set Date/Time");
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
			cursor_position = 0;
			cursor_blink_state = 0;
			Display_Alarm_Setting();
			} else if (key == '3') {
			mode = MODE_SET_TIME;
			// Initialize with current time values
			new_time = RTC_GetTime();
			datetime_setting_field = 0;
			cursor_blink_state = 0;
			Display_DateTime_Setting();
		}
		} else if (mode == MODE_CALCULATOR && key) {
		Calculator_Handler(key);
		} else if (mode == MODE_ALARM && key) {
		Alarm_Input_Handler(key);
		} else if (mode == MODE_SET_TIME && key) {
		DateTime_Input_Handler(key);
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
/////////////////////
// UART functions for serial communication
void UART_init(void);
void UART_transmit(unsigned char data);
void UART_print(const char *str);
void UART_println(const char *str);
void UART_printNum(long num);

// Pin definitions (using port numbers instead of Arduino pin numbers)
#define TRIGGER_PIN PC2    // A2
#define ECHO_PIN    PC3    // A3
#define LED_PIN      PB5    // Digital pin 13 (SCK)
#define ANALOG_PIN   1      // ADC4/PC4

// Thresholds
#define DISTANCE_THRESHOLD 30    // Distance threshold in cm
#define ANALOG_THRESHOLD   300   // Analog sensor threshold

// Timing variables
volatile uint8_t measurement_complete = 0;
volatile uint16_t timer_overflows = 0;
#define MEASUREMENT_INTERVAL 500  // Measurement interval in ms

// Helper function prototypes
long microsecondsToInches(long microseconds);
long microsecondsToCentimeters(long microseconds);
uint16_t readADC(uint8_t channel);
unsigned long pulseIn(uint8_t pin, uint8_t state);
void initTimer1(void);

//////////////////////

int main(void) {
	I2C_Init();
	LCD_Init();
	RTC_Init();
	Keypad_Init();
	Alarm_Init();
	Buzzer_Init();
	
	// Display time and date on startup
	Display_Time_And_Date();
/////////////////////////////
    // Initialize UART for serial communication
    UART_init();
    
    // Setup pins
    // Set trigger pin as output
    DDRC |= (1 << TRIGGER_PIN);
    // Set echo pin as input
    DDRC &= ~(1 << ECHO_PIN);
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Turn off LED initially
    PORTB &= ~(1 << LED_PIN);
    
    // Initialize ADC
    // Set reference voltage to AVCC
    ADMUX = (1 << REFS0);
    // Enable ADC, set prescaler to 128
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Initialize timer for fixed-interval measurements
    initTimer1();
    
    // Enable global interrupts
    sei();
    
    // Print startup message
    UART_println("Combined Ultrasonic and Analog Sensor System");
    UART_println("-------------------------------------------");
    UART_println("LED will turn ON when:");
    UART_println("- Distance < 10 cm AND");
    UART_println("- Analog reading < 300");
    UART_println("");
    UART_println("Measurements taken every 500ms");
    UART_println("");
    
    // Flush any pending transmissions
    _delay_ms(100);
/////////////////////////////
while (1) {
    // Check if alarm triggered
    if (RTC_CheckAlarm()) {
        // Alarm has triggered!
        Buzzer_On();  // Make sure you have this function in alarm.c
        alarm_active = 1;
        
        LCD_Clear();
        LCD_Print("ALARM!");
        LCD_SetCursor(0, 1);
        LCD_Print("Press = to stop");
        
        RTC_ClearAlarm();  // Clear flag for next alarm
            // Switch to alarm mode to enable alarm stopping
    mode = MODE_ALARM;
    }
    
    Mode_Handler();
    Update_Time_Display();
    
    if (mode == MODE_ALARM) {
        Blink_Cursor();
    } else if (mode == MODE_SET_TIME) {
        Blink_Cursor(); 
    }
    
    _delay_ms(50);
}
	return 0;
}

//////////////////////////////

// Initialize Timer1 for fixed interval measurements
void initTimer1(void) {
	// Set Timer1 in CTC mode with OCR1A as top
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	
	// Set OCR1A for 500ms interval with prescaler 256
	// For 16MHz clock: 16,000,000 / 256 = 62,500 ticks per second
	// For 500ms: 62,500 * 0.5 = 31,250
	OCR1A = 31250;
	
	// Enable Timer1 Compare A Match Interrupt
	TIMSK1 = (1 << OCIE1A);
	
	// Start Timer1 with prescaler 256
	TCCR1B |= (1 << CS12);
}

// Timer1 Compare A Match ISR
ISR(TIMER1_COMPA_vect) {
	// Set flag to indicate it's time for a measurement
	measurement_complete = 1;
}

// Helper function to convert microseconds to inches
long microsecondsToInches(long microseconds) {
	// Sound travels 1130 ft/s or 73.746 microseconds per inch
	// Divide by 2 (round trip)
	return microseconds / 74 / 2;
}

// Helper function to convert microseconds to centimeters
long microsecondsToCentimeters(long microseconds) {
	// Sound travels 343 m/s or 29.155 microseconds per centimeter
	// Divide by 2 (round trip)
	return microseconds / 29 / 2;
}

// Read value from ADC channel with noise reduction
uint16_t readADC(uint8_t channel) {
	uint16_t result = 0;
	uint8_t i;
	
	// Take multiple readings and average
	for (i = 0; i < 4; i++) {
		// Select ADC channel
		ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
		
		// Start conversion
		ADCSRA |= (1 << ADSC);
		
		// Wait for conversion to complete
		while (ADCSRA & (1 << ADSC));
		
		// Add to result
		result += ADC;
		
		// Short delay between readings
		_delay_us(50);
	}
	
	// Return average
	return result / 4;
}

// Improved function to measure pulse width with better timing
unsigned long pulseIn(uint8_t pin, uint8_t state) {
	unsigned long width = 0;
	unsigned long timeout = 50000;  // Shorter timeout (about 50ms)
	
	// Wait for pin to enter the specified state
	unsigned long startTime = 0;
	while (((PINC & (1 << pin)) >> pin) != state) {
		startTime++;
		_delay_us(1);
		if (startTime > timeout) return 0;  // Return 0 if timed out
	}
	
	// Measure time in state
	unsigned long pulseStart = 0;
	while (((PINC & (1 << pin)) >> pin) == state) {
		pulseStart++;
		_delay_us(1);
		if (pulseStart > timeout) return 0;  // Return 0 if timed out
		width++;
	}
	
	return width;
}

// Initialize UART (9600 baud with 16MHz clock)
void UART_init(void) {
	// Set baud rate to 9600 (for 16MHz CPU clock)
	UBRR0H = 0;
	UBRR0L = 103;
	
	// Enable transmitter
	UCSR0B = (1 << TXEN0);
	
	// Set frame format: 8 data bits, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmit a byte via UART
void UART_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));
	
	// Put data into buffer
	UDR0 = data;
}

// Send a string via UART
void UART_print(const char *str) {
	while (*str) {
		UART_transmit(*str++);
	}
}

// Send a string with newline via UART
void UART_println(const char *str) {
	UART_print(str);
	UART_transmit('\r');
	UART_transmit('\n');
}

// Print a number via UART
void UART_printNum(long num) {
	char buffer[12];  // Enough for a 32-bit number
	ltoa(num, buffer, 10);
	UART_print(buffer);
}