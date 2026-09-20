#include "paj7620.h"
#include "soft_i2c.h"
#include "delay.h"
#include "stdio.h"
#define PAJ7620_ADDR  0x73

/* 手势标志位定义 */
#define GES_UP_FLAG           (1 << 0)
#define GES_DOWN_FLAG         (1 << 1)
#define GES_LEFT_FLAG         (1 << 2)
#define GES_RIGHT_FLAG        (1 << 3)
#define GES_FORWARD_FLAG      (1 << 4)
#define GES_BACKWARD_FLAG     (1 << 5)

/* 初始化配置数组（来自正点原子） */
static const uint8_t init_reg_array[][2] = {
    {0xEF, 0x00},
    {0x37, 0x07}, {0x38, 0x17}, {0x39, 0x06}, {0x41, 0x00}, {0x42, 0x00},
    {0x46, 0x2D}, {0x47, 0x0F}, {0x48, 0x3C}, {0x49, 0x00}, {0x4A, 0x1E},
    {0x4C, 0x20}, {0x51, 0x10}, {0x5E, 0x10}, {0x60, 0x27}, {0x80, 0x42},
    {0x81, 0x44}, {0x82, 0x04}, {0x8B, 0x01}, {0x90, 0x06}, {0x95, 0x0A},
    {0x96, 0x0C}, {0x97, 0x05}, {0x9A, 0x14}, {0x9C, 0x3F}, {0xA5, 0x19},
    {0xCC, 0x19}, {0xCD, 0x0B}, {0xCE, 0x13}, {0xCF, 0x64}, {0xD0, 0x21},
    {0xEF, 0x01}, {0x02, 0x0F}, {0x03, 0x10}, {0x04, 0x02}, {0x25, 0x01},
    {0x27, 0x39}, {0x28, 0x7F}, {0x29, 0x08}, {0x3E, 0xFF}, {0x5E, 0x3D},
    {0x65, 0x96}, {0x67, 0x97}, {0x69, 0xCD}, {0x6A, 0x01}, {0x6D, 0x2C},
    {0x6E, 0x01}, {0x72, 0x01}, {0x73, 0x35}, {0x74, 0x00}, {0x77, 0x01}
};

/* 手势模式配置数组（来自正点原子） */
static const uint8_t gesture_mode_array[][2] = {
    {0xEF, 0x00}, {0x41, 0x00}, {0x42, 0x00},
    {0xEF, 0x00}, {0x48, 0x3C}, {0x49, 0x00},
    {0x51, 0x10}, {0x83, 0x20}, {0x9F, 0xF9},
    {0xEF, 0x01}, {0x01, 0x1E}, {0x02, 0x0F},
    {0x03, 0x10}, {0x04, 0x02}, {0x41, 0x40},
    {0x43, 0x30}, {0x65, 0x96}, {0x66, 0x00},
    {0x67, 0x97}, {0x68, 0x01}, {0x69, 0xCD},
    {0x6A, 0x01}, {0x6B, 0xB0}, {0x6C, 0x04},
    {0x6D, 0x2C}, {0x6E, 0x01}, {0x74, 0x00},
    {0xEF, 0x00}, {0x41, 0xFF}, {0x42, 0x01}
};

/**
 * @brief  PAJ7620 初始化（使用正点原子序列）
 */
void PAJ7620_Init(void)
{
    uint16_t i;

    /* 1. 唤醒传感器 */
    I2C_WriteReg(0xEF, 0x01);
    HAL_Delay(5);
    I2C_WriteReg(0xEF, 0x01);
    HAL_Delay(10);   // 适当延长等待

    /* 2. 检查唤醒状态（仅作参考，不阻断初始化） */
    uint8_t reg00 = I2C_ReadReg(0x00);
    if (reg00 != 0x20)
    {
        printf("PAJ7620 wakeup status: 0x%02X (expected 0x20), force init...\r\n", reg00);
        // 不 return，继续执行初始化
    }
    else
    {
        printf("PAJ7620 wakeup OK\r\n");
    }

    /* 3. 写入初始化寄存器配置 */
    for (i = 0; i < sizeof(init_reg_array) / 2; i++)
    {
        I2C_WriteReg(init_reg_array[i][0], init_reg_array[i][1]);
    }

    /* 4. 切换到手势检测模式 */
    for (i = 0; i < sizeof(gesture_mode_array) / 2; i++)
    {
        I2C_WriteReg(gesture_mode_array[i][0], gesture_mode_array[i][1]);
    }

    /* 5. 等待传感器就绪 */
    HAL_Delay(50);
    printf("PAJ7620 Init Done\r\n");
		    // 5. 等待传感器就绪
    HAL_Delay(200);
    // 6. 额外清除中断标志
    I2C_WriteReg(0x43, 0x00);
    I2C_WriteReg(0x44, 0x00);
    HAL_Delay(10);
    I2C_WriteReg(0x43, 0x00);
    I2C_WriteReg(0x44, 0x00);
    printf("PAJ7620 Init Done\r\n");
}

/**
 * @brief  PAJ7620 读取手势（使用正点原子方式，读取两个寄存器）
 */
uint8_t PAJ7620_ReadGesture(void)
{
    uint8_t flag1, flag2;
    uint16_t flag;

    flag1 = I2C_ReadReg(0x43);  // 中断标志1
    flag2 = I2C_ReadReg(0x44);  // 中断标志2
    flag = ((uint16_t)flag2 << 8) | flag1;

    // 清除中断标志
    I2C_WriteReg(0x43, 0x00);
    I2C_WriteReg(0x44, 0x00);

    switch (flag)
    {
        case GES_UP_FLAG:          return GESTURE_UP;
        case GES_DOWN_FLAG:        return GESTURE_DOWN;
        case GES_LEFT_FLAG:        return GESTURE_LEFT;
        case GES_RIGHT_FLAG:       return GESTURE_RIGHT;
        case GES_FORWARD_FLAG:     return GESTURE_FORWARD;
        case GES_BACKWARD_FLAG:    return GESTURE_BACKWARD;
        default:                   return GESTURE_NONE;
    }
}

/* 底层读写函数（保持不变，提供给外部诊断） */
uint8_t I2C_ReadReg(uint8_t reg)
{
    uint8_t data;
    I2C_Start();
    I2C_SendByte(PAJ7620_ADDR << 1);
    I2C_WaitAck();
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte((PAJ7620_ADDR << 1) | 1);
    I2C_WaitAck();
    data = I2C_ReadByte(0);
    I2C_Stop();
    return data;
}

void I2C_WriteReg(uint8_t reg, uint8_t val)
{
    I2C_Start();
    I2C_SendByte(PAJ7620_ADDR << 1);
    I2C_WaitAck();
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_SendByte(val);
    I2C_WaitAck();
    I2C_Stop();
}
