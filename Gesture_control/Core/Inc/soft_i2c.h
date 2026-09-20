#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H
#include "main.h"

#define I2C_SCL_PIN  GPIO_PIN_6
#define I2C_SDA_PIN  GPIO_PIN_7
#define I2C_PORT     GPIOB

void Soft_I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReadByte(uint8_t ack);
uint8_t I2C_WaitAck(void);
#endif
