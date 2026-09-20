/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 手势控制舵机项目 - 最终完整版
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "delay.h"
#include "soft_i2c.h"
#include "paj7620.h"
#include "oled.h"
#include "servo.h"
#include "filter.h"
#include "state_machine.h"
#include "recorder.h"
#include "motion.h"
#include "menu.h"
#include "config.h"
#include <stdio.h>

/* 重定向 printf 到串口 */
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

/* Private variables ---------------------------------------------------------*/
volatile uint32_t system_tick_ms = 0;   // 全局毫秒计数器

void SystemClock_Config(void);

/**
  * @brief  The application entry point.
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  /* 初始化 CubeMX 生成的外设 */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();

  /* 初始化自定义外设和模块 */
  delay_init();
  Soft_I2C_Init();
  PAJ7620_Init();                 // 传感器初始化（正点原子序列）
  OLED_Init();                    // OLED 初始化
  OLED_Clear();
  OLED_ShowString(0, 0, "Gesture Servo");
  Servo_Init();  	// 舵机启动并居中
  Filter_Init(config.filter_window);
  StateMachine_Init();
  Recorder_Init();
  Motion_Init();
  Menu_Init();                    // 菜单初始化
  Config_Load();                  // 从 Flash 加载配置
  printf("Filter window = %d\r\n", config.filter_window);
  if (config.filter_window < 1 || config.filter_window > 20) {
      config.filter_window = 5;
  }
  Filter_Init(config.filter_window);
  printf("System started.\r\n");

  /* 主循环（非阻塞） */
  #define GESTURE_INTERVAL_MS  20
  #define MOTION_INTERVAL_MS   20

  while (1)
  {
    static uint32_t last_led_time = 0;
    static uint32_t last_print_time = 0;
    static uint32_t last_motion_time = 0;
    static uint32_t last_gesture_time = 0;
    static uint8_t led_state = 0;
   // static uint32_t motion_call_count = 0;

    uint8_t raw_gesture, stable_gesture;
    uint8_t is_long_press = 0;

    /* 运动更新 & 回放驱动 */
    if (system_tick_ms - last_motion_time >= MOTION_INTERVAL_MS) {
      last_motion_time = system_tick_ms;
  //    motion_call_count++;
      Motion_Update();
			//printf("Motion idle: %d\r\n", Motion_IsIdle());
      if (StateMachine_GetState() == STATE_PLAYBACK) {
        Recorder_UpdatePlayback();
      }
    }
    /* 手势读取 & 滤波 */
    if (system_tick_ms - last_gesture_time >= GESTURE_INTERVAL_MS) {
      last_gesture_time = system_tick_ms;
      raw_gesture = PAJ7620_ReadGesture();
      Filter_AddGesture(raw_gesture);

      /* 保留原始手势打印，便于观察传感器是否工作 */
      if (raw_gesture != GESTURE_NONE) {
        const char *gest_names[] = {"NONE", "UP", "DOWN", "LEFT", "RIGHT", "FORWARD", "BACKWARD"};
        printf("Raw: %s\r\n", gest_names[raw_gesture]);
      }
    }

    /* 获取滤波后的稳定手势（不再绕过滤波） */
    stable_gesture = Filter_GetStableGesture();
       /* 手势读取 & 滤波 
    if (system_tick_ms - last_gesture_time >= GESTURE_INTERVAL_MS) {
      last_gesture_time = system_tick_ms;
      raw_gesture = PAJ7620_ReadGesture();
      Filter_AddGesture(raw_gesture);

      if (raw_gesture != GESTURE_NONE) {
        const char *gest_names[] = {"NONE", "UP", "DOWN", "LEFT", "RIGHT", "FORWARD", "BACKWARD"};
        printf("Raw gesture: %s\r\n", gest_names[raw_gesture]);
      }

      //临时：绕过滤波，直接用原始手势 
      stable_gesture = raw_gesture;
    }
    else {
      stable_gesture = GESTURE_NONE;
    }
    // 注释掉原来的 stable_gesture = Filter_GetStableGesture();
*/
    /* 长按检测 */
    StateMachine_UpdateLongPress(stable_gesture, &is_long_press);

    /* 状态机处理 */
    StateMachine_Process(stable_gesture, is_long_press);

      /* 菜单处理（仅菜单状态有效） */
    static uint8_t was_menu = 0;
    if (StateMachine_GetState() == STATE_MENU) {
      was_menu = 1;
      Menu_ProcessGesture(stable_gesture);
    }
    else {
      /* 刚退出菜单时，清屏并重绘主界面 */
      if (was_menu) {
          OLED_Clear();
          OLED_ShowString(0, 0, "Gesture Servo");
          was_menu = 0;
      }
      /* 显示当前模式（第二行） */
      char buf[22];
      switch (StateMachine_GetState()) {
          case STATE_CONTROL:  sprintf(buf, "Control   "); break;
          case STATE_RECORD:   sprintf(buf, "Recording "); break;
          case STATE_PLAYBACK: sprintf(buf, "Playback  "); break;
      }
      OLED_ShowString(0, 2, buf);
    }

    /* LED 闪烁（每 500ms 翻转一次） */
    if (system_tick_ms - last_led_time >= 500) {
      last_led_time = system_tick_ms;
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      led_state = !led_state;
    }

    /* 每秒串口输出系统状态 */
    if (system_tick_ms - last_print_time >= 1000) {
      last_print_time = system_tick_ms;
    // printf("Tick: %u ms, LED: %s, State: %d, MotionCalls: %u\r\n",
      //       (unsigned int)system_tick_ms,
      //       led_state ? "ON" : "OFF",
       //      StateMachine_GetState(),
       //      (unsigned int)motion_call_count);
 //     motion_call_count = 0;
    }
  }
}

/* 系统时钟配置（HSE 8MHz -> 72MHz） */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
