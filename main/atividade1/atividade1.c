#include "atividade1.h"

static const char *TAG_CB = "Circular Buffer";

circular_buffer_t cb;

SemaphoreHandle_t xCircularBufferMutex = NULL;
SemaphoreHandle_t xEmptySlotsSem       = NULL;
SemaphoreHandle_t xFilledSlotsSem      = NULL;

void atividade1(void) {
    ESP_LOGI("Atividade 1", "Rodando atividade 1");

    cb_init();

    xCircularBufferMutex = xSemaphoreCreateMutex();
    xEmptySlotsSem       = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    xFilledSlotsSem      = xSemaphoreCreateCounting(BUFFER_SIZE, 0);

    xTaskCreatePinnedToCore(task_read, "Read", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_write, "Write", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 1);
}

/* Circular Buffer Implementation */
void cb_init(void) {
    cb.count = 0;
    cb.head  = 0;
    cb.tail  = 0;
}

bool cb_full() { return cb.count == BUFFER_SIZE; }

bool cb_empty() { return cb.count == 0; }

bool cb_push(int in_data) {
    if (cb_full()) {
        ESP_LOGE(TAG_CB, "Circular Buffer is full.");
        return false;
    }

    cb.buffer[cb.head] = in_data;

    cb.head = (cb.head + 1) % BUFFER_SIZE;
    cb.count++;

    return true;
}

bool cb_pop(int *out_data) {
    if (cb_empty()) {
        ESP_LOGE(TAG_CB, "Circular Buffer is empty.");
        return false;
    }

    *out_data = cb.buffer[cb.tail];

    cb.tail = (cb.tail + 1) % BUFFER_SIZE;
    cb.count--;

    return true;
}

/* Tasks */
void task_read(void *pvParameters) {
    int out_data;
    while (true) {
        xSemaphoreTake(xFilledSlotsSem, portMAX_DELAY);

        xSemaphoreTake(xCircularBufferMutex, portMAX_DELAY);
        cb_pop(&out_data);
        xSemaphoreGive(xCircularBufferMutex);

        xSemaphoreGive(xEmptySlotsSem);
        ESP_LOGI("Read", "Data read: %d", out_data);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void task_write(void *pvParameters) {
    while (true) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            xSemaphoreTake(xEmptySlotsSem, portMAX_DELAY);

            xSemaphoreTake(xCircularBufferMutex, portMAX_DELAY);
            cb_push(i);
            xSemaphoreGive(xCircularBufferMutex);

            xSemaphoreGive(xFilledSlotsSem);

            ESP_LOGI("Write", "Data written: %d", i);

            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}
