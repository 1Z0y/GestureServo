#ifndef __MOTION_H
#define __MOTION_H
#include "main.h"

void Motion_Init(void);
void Motion_SetTarget(uint8_t angle);
void Motion_Update(void);
uint8_t Motion_IsIdle(void);
#endif
