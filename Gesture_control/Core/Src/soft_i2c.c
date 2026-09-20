#include "soft_i2c.h"
#include "delay.h"

void Soft_I2C_Init(void) {
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN | I2C_SDA_PIN, GPIO_PIN_SET);
    delay_us(5);
}

static void I2C_Delay(void) {
    delay_us(5);
}

void I2C_Start(void) {
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Delay();
}

void I2C_Stop(void) {
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
}

void I2C_SendByte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        if (byte & 0x80)
            HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
        I2C_Delay();
        HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
        I2C_Delay();
        HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
        I2C_Delay();
        byte <<= 1;
    }
    // Release SDA for ACK
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
}

uint8_t I2C_ReadByte(uint8_t ack) {
    uint8_t data = 0;
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
        I2C_Delay();
        if (HAL_GPIO_ReadPin(I2C_PORT, I2C_SDA_PIN) == GPIO_PIN_SET)
            data |= 0x01;
        HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
        I2C_Delay();
    }
    // ACK/NACK
    if (ack)
        HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    return data;
}

uint8_t I2C_WaitAck(void) {
    uint8_t ack;
    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    ack = (HAL_GPIO_ReadPin(I2C_PORT, I2C_SDA_PIN) == GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    return ack;
}
