#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

// I2C LCD configuration (PCF8574)
#define LCD_ADDRESS 0x27  // Default I2C address for PCF8574 (adjust if needed)

// PCF8574 pin mapping for LCD
#define RS_PIN 0  // P0: RS
#define RW_PIN 1  // P1: RW
#define EN_PIN 2  // P2: EN
#define BL_PIN 3  // P3: Backlight
#define D4_PIN 4  // P4: D4
#define D5_PIN 5  // P5: D5
#define D6_PIN 6  // P6: D6
#define D7_PIN 7  // P7: D7

// Function Prototypes
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_String(const char *str);

#endif