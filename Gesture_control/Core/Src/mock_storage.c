#include "mock_storage.h"
#include <string.h>

#ifdef USE_MOCK
uint8_t fake_flash[FAKE_FLASH_SIZE] = {0xFF}; // 初始化为全 0xFF

void Mock_Flash_Write(uint32_t address, uint16_t data)
{
    if (address < FAKE_FLASH_SIZE - 1) {
        fake_flash[address] = (uint8_t)(data & 0xFF);
        fake_flash[address + 1] = (uint8_t)((data >> 8) & 0xFF);
    }
}

uint16_t Mock_Flash_Read(uint32_t address)
{
    uint16_t value = 0xFFFF;
    if (address < FAKE_FLASH_SIZE - 1) {
        value = fake_flash[address] | (fake_flash[address + 1] << 8);
    }
    return value;
}

void Mock_Flash_ErasePage(uint32_t page_address)
{
    // 简单模拟：将整个数组擦除为 0xFF
    memset(fake_flash, 0xFF, FAKE_FLASH_SIZE);
}
#endif
