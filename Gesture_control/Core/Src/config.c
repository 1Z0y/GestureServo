#include "config.h"
#include "mock_storage.h"   // ?? USE_MOCK ???

Config_t config = {5, 0, 180};   // ???

/* ????? */
static uint16_t Config_Checksum(Config_t *cfg) {
    return cfg->filter_window + cfg->angle_min + cfg->angle_max + 0xA5;
}

/* ?????????? */
static void Config_Validate(Config_t *cfg) {
    if (cfg->filter_window < 1 || cfg->filter_window > 20) {
        cfg->filter_window = 5;   // ????
    }
    if (cfg->angle_min > 180) cfg->angle_min = 0;
    if (cfg->angle_max > 180) cfg->angle_max = 180;
    if (cfg->angle_min > cfg->angle_max) {
        cfg->angle_min = 0;
        cfg->angle_max = 180;
    }
}

void Config_Load(void)
{
#ifdef USE_MOCK
    uint16_t val;
    Config_t temp;
    val = Mock_Flash_Read(CONFIG_FLASH_ADDR);
    temp.filter_window = (uint8_t)val;
    val = Mock_Flash_Read(CONFIG_FLASH_ADDR + 2);
    temp.angle_min = (uint8_t)val;
    val = Mock_Flash_Read(CONFIG_FLASH_ADDR + 4);
    temp.angle_max = (uint8_t)val;
    uint16_t crc = Mock_Flash_Read(CONFIG_FLASH_ADDR + 6);
    if (Config_Checksum(&temp) == crc) {
        config = temp;
    } else {
        Config_Reset();
    }
#else
    uint16_t *p = (uint16_t*)CONFIG_FLASH_ADDR;
    Config_t temp;
    temp.filter_window = (uint8_t)(*p);
    temp.angle_min = (uint8_t)(*(p+1));
    temp.angle_max = (uint8_t)(*(p+2));
    uint16_t stored_crc = *(p+3);
    if (Config_Checksum(&temp) == stored_crc) {
        config = temp;
    } else {
        Config_Reset();
    }
#endif

    /* ??????,?????????? */
    Config_Validate(&config);
}

void Config_Save(void)
{
    /* ????????,??????? */
    Config_Validate(&config);

#ifdef USE_MOCK
    Mock_Flash_ErasePage(CONFIG_FLASH_ADDR);
    Mock_Flash_Write(CONFIG_FLASH_ADDR, config.filter_window);
    Mock_Flash_Write(CONFIG_FLASH_ADDR + 2, config.angle_min);
    Mock_Flash_Write(CONFIG_FLASH_ADDR + 4, config.angle_max);
    Mock_Flash_Write(CONFIG_FLASH_ADDR + 6, Config_Checksum(&config));
#else
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = CONFIG_FLASH_ADDR;
    EraseInitStruct.NbPages = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, CONFIG_FLASH_ADDR, config.filter_window);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, CONFIG_FLASH_ADDR + 2, config.angle_min);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, CONFIG_FLASH_ADDR + 4, config.angle_max);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, CONFIG_FLASH_ADDR + 6, Config_Checksum(&config));
    HAL_FLASH_Lock();
#endif
}

void Config_Reset(void)
{
    config.filter_window = 5;
    config.angle_min = 0;
    config.angle_max = 180;
    Config_Save();   // ???????
}
