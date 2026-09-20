#include "state_machine.h"
#include "servo.h"
#include "recorder.h"
#include "motion.h"
#include "menu.h"
#include "config.h"
#include <stdio.h>

static State_t current_state = STATE_CONTROL;
static uint8_t last_gesture = GESTURE_NONE;
static uint32_t long_press_start = 0;
static uint8_t long_press_detected = 0;

void StateMachine_Init(void) {
    current_state = STATE_CONTROL;
    printf("\r\n[SM] Init -> STATE_CONTROL\r\n");
}

void StateMachine_UpdateLongPress(uint8_t gesture, uint8_t *is_long) {
    static uint8_t latched_gesture = GESTURE_NONE;  // 锁存的手势
    static uint32_t latch_timestamp = 0;            // 锁存时间

    *is_long = 0;

    // 1. 更新锁存：当有新手势时，刷新锁存并重启计时
    if (gesture != GESTURE_NONE) {
        latched_gesture = gesture;
        latch_timestamp = system_tick_ms;
    }
    // 2. 如果锁存的手势超过2秒未被刷新，则清空（超时）
    else if (latched_gesture != GESTURE_NONE && (system_tick_ms - latch_timestamp > 2000)) {
        latched_gesture = GESTURE_NONE;
    }

    // 3. 使用锁存的手势进行长按检测
    if (latched_gesture != GESTURE_NONE) {
        if (latched_gesture == last_gesture) {
            if (system_tick_ms - long_press_start >= LONG_PRESS_MS) {
                if (!long_press_detected) {
                    *is_long = 1;
                    long_press_detected = 1;
                    printf("[SM] Long press detected: gesture=%d\r\n", latched_gesture);
                }
            }
        } else {
            // 手势变化，重置长按计时
            last_gesture = latched_gesture;
            long_press_start = system_tick_ms;
            long_press_detected = 0;
        }
    } else {
        // 无手势，重置
        last_gesture = GESTURE_NONE;
        long_press_start = system_tick_ms;
        long_press_detected = 0;
    }
		
}
static uint8_t GestureToAngle(uint8_t gesture) {
    switch (gesture) {
        case GESTURE_LEFT:  return 0;
        case GESTURE_RIGHT: return 180;
        case GESTURE_UP:
        case GESTURE_DOWN:  return 90;
        default: return 0xFF;
    }
}
void StateMachine_Process(uint8_t gesture, uint8_t is_long_press) {
    static uint8_t prev_control_gesture = GESTURE_NONE;
    static uint8_t prev_record_gesture = GESTURE_NONE;
    static uint8_t last_valid_gesture = GESTURE_NONE;  // 新增

    uint8_t angle;
    switch (current_state) {

        case STATE_CONTROL:
            // 记录最近的有效手势
            if (gesture != GESTURE_NONE) {
                last_valid_gesture = gesture;
            }

            // 手势不变且非长按则跳过
            if (gesture == prev_control_gesture && !is_long_press) return;
            prev_control_gesture = gesture;

            if (is_long_press) {
                // 使用 last_valid_gesture 判断用户意图
                if (last_valid_gesture == GESTURE_FORWARD) {
                    current_state = STATE_MENU;
                    printf("\r\n========== MENU ENTERED ==========\r\n");
                    Menu_Enter();
                    // 重置状态，防止误触发
                    last_valid_gesture = GESTURE_NONE;
                } else if (last_valid_gesture == GESTURE_UP) {
                    current_state = STATE_RECORD;
                    printf("\r\n========== RECORDING STARTED ==========\r\n");
                    Recorder_StartRecord();
                    prev_record_gesture = GESTURE_NONE;
                    last_valid_gesture = GESTURE_NONE;
                } else if (last_valid_gesture == GESTURE_DOWN) {
                    if (Recorder_GetCount() > 0) {
                        current_state = STATE_PLAYBACK;
                        printf("\r\n========== PLAYBACK STARTED ==========\r\n");
                        Recorder_StartPlayback();
                    }
                    last_valid_gesture = GESTURE_NONE;
                }
            } else if (gesture != GESTURE_NONE) {
                angle = GestureToAngle(gesture);
                if (angle != 0xFF) {
                    printf("[SM] CONTROL gesture=%d -> angle=%d\r\n", gesture, angle);
                    Motion_SetTarget(angle);
                }
            }
            break;

               case STATE_RECORD:
            // 记录最近有效手势（供停止时使用）
            if (gesture != GESTURE_NONE) {
                last_valid_gesture = gesture;
            }
            // 长按 UP 或 DOWN 停止录制（使用锁存的最近手势）
            if (is_long_press && (last_valid_gesture == GESTURE_UP || last_valid_gesture == GESTURE_DOWN)) {
                current_state = STATE_CONTROL;
                printf("\r\n========== RECORDING STOPPED ==========\r\n");
                Recorder_StopRecord();
                last_valid_gesture = GESTURE_NONE;
                break;
            }
            // 手势不变则跳过（避免重复记录）
            if (gesture == prev_record_gesture) return;
            prev_record_gesture = gesture;

            if (gesture != GESTURE_NONE) {
                angle = GestureToAngle(gesture);
                if (angle != 0xFF) {
                    printf("[SM] RECORD gesture=%d -> angle=%d\r\n", gesture, angle);
                    Motion_SetTarget(angle);
                    if (Recorder_GetCount() < REC_MAX) {
                        Recorder_RecordAngle(angle);
                    } else {
                        printf("[REC] Recording full!\r\n");
                    }
                }
            }
            break;

  case STATE_PLAYBACK:
    // 记录最近有效手势
    if (gesture != GESTURE_NONE) {
        last_valid_gesture = gesture;
    }
    // 长按 FORWARD 或 BACKWARD 停止回放
    if (is_long_press && (last_valid_gesture == GESTURE_FORWARD || last_valid_gesture == GESTURE_BACKWARD)) {
        printf("\r\n========== PLAYBACK STOPPED BY USER ==========\r\n");
        Recorder_StopPlayback();
        current_state = STATE_CONTROL;
        last_valid_gesture = GESTURE_NONE;
        break;
    }
    // 回放自然结束
    if (Recorder_IsPlaybackDone()) {
        printf("\r\n========== PLAYBACK COMPLETED ==========\r\n");
        current_state = STATE_CONTROL;
    }
    break;
        case STATE_MENU:
            if (Menu_ShouldExit()) {
                printf("\r\n========== MENU EXITED ==========\r\n");
                current_state = STATE_CONTROL;
                Menu_Exit();
            }
            break;
    }
}

State_t StateMachine_GetState(void) {
    return current_state;
}
