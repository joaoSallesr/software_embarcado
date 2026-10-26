#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#define BT_GPIO   16
#define LED1_GPIO 17
#define LED2_GPIO 18

void aula17(void);
void led_blink(TimerHandle_t xTimer);
void task_button(void *pvParameters);
void led_off(TimerHandle_t xTimer);
