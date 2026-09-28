#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} circular_buffer_t;

extern circular_buffer_t cb;

extern SemaphoreHandle_t xCircularBufferMutex;
extern SemaphoreHandle_t xEmptySlotsSem;
extern SemaphoreHandle_t xFilledSlotsSem;

void cb_init(void);
bool cb_full(void);
bool cb_empty(void);
bool cb_push(int in_data);
bool cb_pop(int *out_data);

void task_read(void *pvParameters);
void task_write(void *pvParameters);

void atividade1(void);
