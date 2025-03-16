
/*
 * i2c.h
 *
 * Created: 3/15/2025 11:44:43 AM
 *  Author: NC
 */ 
#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(uint8_t data);
uint8_t I2C_ReadAck(void);
uint8_t I2C_ReadNack(void);

#endif
