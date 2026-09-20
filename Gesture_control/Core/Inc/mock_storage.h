#ifndef __MOCK_STORAGE_H
#define __MOCK_STORAGE_H
#include "main.h"

#ifdef USE_MOCK
#define FAKE_FLASH_SIZE 2048
extern uint8_t fake_flash[FAKE_FLASH_SIZE];

void Mock_Flash_Write(uint32_t address, uint16_t data);
uint16_t Mock_Flash_Read(uint32_t address);
void Mock_Flash_ErasePage(uint32_t page_address);
#endif

#endif
