#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <dht.h>

#define DHT_GPIO GPIO_NUM_4

extern QueueHandle_t xDataQueue;

typedef struct {
    float humidity;
    float temperature;
} data_t;

void atv2_ex1(void);

void task_read(void *pvParameters);
void task_humid_mean(void *pvParameters);
void task_temp_mean(void *pvParameters);
void task_log(void *pvParameters);