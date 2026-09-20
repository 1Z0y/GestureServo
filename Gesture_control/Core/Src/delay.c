#include "delay.h"
        

static uint32_t fac_us = 0;

void delay_init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    fac_us = SystemCoreClock / 1000000;
}

void delay_us(uint32_t nus) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = nus * fac_us;
    while ((DWT->CYCCNT - start) < ticks);
}

void delay_ms(uint32_t nms) {
    uint32_t start = system_tick_ms;
    while (system_tick_ms - start < nms);
}
