#ifndef __MOCK_DISPLAY_H
#define __MOCK_DISPLAY_H
#include "main.h"

#ifdef USE_MOCK
void Mock_OLED_ShowString(uint8_t line, const char *text);
#endif

#endif
