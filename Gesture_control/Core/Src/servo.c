#include "servo.h"
#ifdef USE_MOCK
#include "mock_servo.h"
#else
#include "tim.h"
#endif

void Servo_Init(void)
{
#ifdef USE_MOCK
    // 无需操作
#else
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    Servo_SetAngle(90);
#endif
}

void Servo_SetAngle(uint8_t angle)
{
#ifdef USE_MOCK
    Mock_Servo_SetAngle(angle);
#else
    if (angle > 180) angle = 180;
    uint16_t pulse = 500 + (uint32_t)angle * 2000 / 180;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pulse);
#endif
}
