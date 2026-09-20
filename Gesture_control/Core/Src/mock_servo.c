#include "mock_servo.h"
#include <stdio.h>

#ifdef USE_MOCK
void Mock_Servo_SetAngle(uint8_t angle) {
    static uint8_t last_angle = 0xFF;
    if (angle != last_angle) {
        printf("[SERVO] %d deg\n", angle);
        last_angle = angle;
    }
}
#endif
