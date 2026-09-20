#ifndef __CONFIG_H
#define __CONFIG_H
#include "main.h"

/* Flash ????(??????) */
#define CONFIG_FLASH_ADDR  0x0800FC00

/* ?????? */
typedef struct {
    uint8_t filter_window;
    uint8_t angle_min;
    uint8_t angle_max;
} Config_t;

extern Config_t config;

void Config_Load(void);
void Config_Save(void);
void Config_Reset(void);

#endif
