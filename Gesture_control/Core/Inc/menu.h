#ifndef __MENU_H
#define __MENU_H
#include "main.h"

typedef enum {
    MENU_TYPE_SUBMENU,
    MENU_TYPE_VALUE,
    MENU_TYPE_ACTION
} MenuItemType;

typedef struct MenuItem {
    char *name;
    MenuItemType type;
    void *value;       // 指向 uint8_t 变量
    uint8_t min, max;
    void (*action)(void);
    struct MenuItem *submenu;
    uint8_t submenu_size;
} MenuItem;

void Menu_Init(void);
void Menu_Enter(void);
void Menu_Exit(void);
void Menu_ProcessGesture(uint8_t gesture);
uint8_t Menu_ShouldExit(void);
#endif
