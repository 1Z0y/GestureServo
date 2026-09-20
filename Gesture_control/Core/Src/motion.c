#include "motion.h"
#include "servo.h"

static uint8_t current_angle = 90;
static int16_t target_angle = 90;
static int16_t step_count = 0;
static int8_t direction = 0;

void Motion_Init(void) {
    Servo_SetAngle(90);
}

void Motion_SetTarget(uint8_t angle) {
    if (angle > 180) angle = 180;
    target_angle = angle;
    int16_t diff = target_angle - current_angle;
    if (diff == 0) return;
    direction = (diff > 0) ? 1 : -1;
    step_count = (diff > 0) ? diff : -diff;
}

void Motion_Update(void) {
    if (step_count > 0) {
        current_angle += direction;
        Servo_SetAngle(current_angle);
        step_count--;
    }
}

uint8_t Motion_IsIdle(void) {
    return (step_count == 0);
}
