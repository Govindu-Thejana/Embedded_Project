#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

#define LCD_ADDR 0x20  // I2C Address of LCD

void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Print(char *str);
void LCD_SetCursor(uint8_t col, uint8_t row);
void LCD_Clear(void);

#endif
