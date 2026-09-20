#ifndef __STATE_MACHINE_H
#define __STATE_MACHINE_H
#include "main.h"

typedef enum {
    STATE_CONTROL,
    STATE_RECORD,
    STATE_PLAYBACK,
    STATE_MENU
} State_t;

void StateMachine_Init(void);
void StateMachine_UpdateLongPress(uint8_t gesture, uint8_t *is_long);
void StateMachine_Process(uint8_t gesture, uint8_t is_long_press);
State_t StateMachine_GetState(void);
#endif
