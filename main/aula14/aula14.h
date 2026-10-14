#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define BT_GPIO  16
#define LED_GPIO 17

void aula14(void);
void task_led(void *pvParamenters);