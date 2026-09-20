#ifndef __FILTER_H
#define __FILTER_H
#include "main.h"

void Filter_Init(uint8_t window_size);
void Filter_AddGesture(uint8_t gesture);
uint8_t Filter_GetStableGesture(void);
#endif
