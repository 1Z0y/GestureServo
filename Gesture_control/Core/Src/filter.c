#include "filter.h"

#define FILTER_BUFFER_MAX 20
static uint8_t buffer[FILTER_BUFFER_MAX];
static uint8_t window = 5;
static uint8_t idx = 0;
static uint8_t count = 0;

void Filter_Init(uint8_t window_size) {
    if (window_size > FILTER_BUFFER_MAX) window_size = FILTER_BUFFER_MAX;
    if (window_size < 1) window_size = 1;
    window = window_size;
    for (uint8_t i = 0; i < FILTER_BUFFER_MAX; i++) buffer[i] = GESTURE_NONE;
}

void Filter_AddGesture(uint8_t gesture) {
    buffer[idx] = gesture;
    idx = (idx + 1) % window;
    count++;
}

uint8_t Filter_GetStableGesture(void) {
    uint8_t vote[7] = {0}; // gesture types up to 6
    uint8_t valid_count = 0;
    for (uint8_t i = 0; i < window; i++) {
        uint8_t g = buffer[i];
        if (g != GESTURE_NONE) {
            vote[g]++;
            valid_count++;
        }
    }
    uint8_t max_vote = 0;
    uint8_t max_gesture = GESTURE_NONE;
    for (uint8_t i = 1; i <= 6; i++) {
        if (vote[i] > max_vote) {
            max_vote = vote[i];
            max_gesture = i;
        }
    }
    // 多数表决：票数 > 窗口一半
       if (max_vote >= 1) return max_gesture;   // 至少有一票就输出
    return GESTURE_NONE;
}
