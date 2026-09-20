#ifndef __PAJ7620_H
#define __PAJ7620_H
#include "main.h"

void PAJ7620_Init(void);
uint8_t PAJ7620_ReadGesture(void);
uint8_t I2C_ReadReg(uint8_t reg);
void I2C_WriteReg(uint8_t reg, uint8_t val);
#endif
