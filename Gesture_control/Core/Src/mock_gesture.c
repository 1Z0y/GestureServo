#include "mock_gesture.h"

#ifdef USE_MOCK

static const uint8_t gesture_sequence[] = {
    GESTURE_UP, GESTURE_LEFT, GESTURE_RIGHT, GESTURE_DOWN,
    GESTURE_LEFT, GESTURE_DOWN, GESTURE_FORWARD, GESTURE_BACKWARD
};
#define SEQ_LEN (sizeof(gesture_sequence) / sizeof(gesture_sequence[0]))

static const uint16_t gesture_durations[] = {
    2000,   // UP     长按 → 进入录制
    1000,   // LEFT   录制 0°
    1000,   // RIGHT  录制 180°
    2000,   // DOWN   长按 → 停止录制
    500,    // LEFT   过渡
    2000,   // DOWN   长按 → 开始回放
    2000,   // FORWARD 长按 → 进入菜单
    1000    // BACKWARD
};

uint8_t Mock_ReadGesture(void)
{
    static uint8_t current_idx = 0;
    static uint32_t last_switch_time = 0;
    uint32_t now = system_tick_ms;

    if (last_switch_time == 0) {
        last_switch_time = now;
    }

    uint8_t current_gesture = gesture_sequence[current_idx];

    if (now - last_switch_time >= gesture_durations[current_idx]) {
        current_idx = (current_idx + 1) % SEQ_LEN;
        last_switch_time = now;
        current_gesture = gesture_sequence[current_idx];
    }

    return current_gesture;
}

#endif
