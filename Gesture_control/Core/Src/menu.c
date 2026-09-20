#include "menu.h"
#include "oled.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

static MenuItem* current_menu = NULL;
static uint8_t selected_index = 0;
static uint8_t menu_exit_flag = 0;

// 示例顶层菜单项
static void ActionResetConfig(void) {
    Config_Reset();
}

static MenuItem main_menu[] = {
    {"Filter Window", MENU_TYPE_VALUE, &config.filter_window, 1, 10, NULL, NULL, 0},
    {"Angle Min",     MENU_TYPE_VALUE, &config.angle_min, 0, 90, NULL, NULL, 0},
    {"Angle Max",     MENU_TYPE_VALUE, &config.angle_max, 90, 180, NULL, NULL, 0},
    {"Reset Config",  MENU_TYPE_ACTION, NULL, 0, 0, ActionResetConfig, NULL, 0},
    {"Back",          MENU_TYPE_ACTION, NULL, 0, 0, NULL, NULL, 0}
};
#define MAIN_MENU_SIZE (sizeof(main_menu)/sizeof(MenuItem))

void Menu_Init(void) {
    current_menu = NULL;
}

void Menu_Enter(void) {
    current_menu = main_menu;
    selected_index = 0;
    menu_exit_flag = 0;
    printf("[MENU] Enter\r\n");

    // 显示当前菜单项和数值
    OLED_Clear();
    MenuItem *item = &current_menu[selected_index];
    OLED_ShowString(0, 0, item->name);
    if (item->type == MENU_TYPE_VALUE) {
        char val_buf[12];
        sprintf(val_buf, "Value: %d", *(uint8_t*)item->value);
        OLED_ShowString(0, 1, val_buf);
    }
}

void Menu_Exit(void) {
    current_menu = NULL;
    menu_exit_flag = 0;
}

uint8_t Menu_ShouldExit(void) {
    return menu_exit_flag;
}

void Menu_ProcessGesture(uint8_t gesture) {
    if (!current_menu) return;

    // 手势去抖：同一手势不连续处理
    static uint8_t prev_gesture_menu = GESTURE_NONE;
    if (gesture == prev_gesture_menu && gesture != GESTURE_NONE) return;
    prev_gesture_menu = gesture;

    if (gesture == GESTURE_NONE) return;

    MenuItem *item = &current_menu[selected_index];
    uint8_t need_refresh = 0;

    switch (gesture) {
        case GESTURE_UP:
            if (selected_index > 0) {
                selected_index--;
                printf("[MENU] Select: %s (index %d)\r\n", item->name, selected_index);
                need_refresh = 1;
            }
            break;

        case GESTURE_DOWN:
            if (selected_index < MAIN_MENU_SIZE - 1) {
                selected_index++;
                printf("[MENU] Select: %s (index %d)\r\n", item->name, selected_index);
                need_refresh = 1;
            }
            break;

        case GESTURE_LEFT:
            if (item->type == MENU_TYPE_VALUE && *(uint8_t*)item->value > item->min) {
                (*(uint8_t*)item->value)--;
                printf("[MENU] %s: %d (dec)\r\n", item->name, *(uint8_t*)item->value);
                Config_Save();
                need_refresh = 1;
            }
            break;

        case GESTURE_RIGHT:
            if (item->type == MENU_TYPE_VALUE && *(uint8_t*)item->value < item->max) {
                (*(uint8_t*)item->value)++;
                printf("[MENU] %s: %d (inc)\r\n", item->name, *(uint8_t*)item->value);
                Config_Save();
                need_refresh = 1;
            }
            break;

        case GESTURE_FORWARD:
            if (item->type == MENU_TYPE_ACTION) {
                if (item->action) {
                    item->action();
                    printf("[MENU] Action: %s\r\n", item->name);
                }
                if (item->action == NULL && strcmp(item->name, "Back") == 0) {
                    menu_exit_flag = 1;
                    printf("[MENU] Back selected, will exit\r\n");
                }
                need_refresh = 1; // 动作执行后可能需要刷新显示
            }
            // 对于数值类型，FORWARD 不再需要，可直接忽略
            break;

        case GESTURE_BACKWARD:
            menu_exit_flag = 1;
            printf("[MENU] Backward gesture, exit menu\r\n");
            break;
    }

    // 刷新显示
    if (need_refresh && !menu_exit_flag) {
        OLED_Clear();
        item = &current_menu[selected_index]; // 更新指针
        OLED_ShowString(0, 0, item->name);
        if (item->type == MENU_TYPE_VALUE) {
            char val_buf[12];
            sprintf(val_buf, "Value: %d", *(uint8_t*)item->value);
            OLED_ShowString(0, 1, val_buf);
        }
    }
}
