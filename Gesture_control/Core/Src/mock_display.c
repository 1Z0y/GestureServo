#include "mock_display.h"
#include <stdio.h>

#ifdef USE_MOCK
void Mock_OLED_ShowString(uint8_t line, const char *text)
{
    printf("[OLED L%d] %s\n", line, text);
}
#endif
