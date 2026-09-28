#include "atividade1.h"

static const char *TAG_CB = "Circular Buffer";

circular_buffer_t cb;

SemaphoreHandle_t xCircularBufferMutex = NULL;
SemaphoreHandle_t xEmptySlotsSem       = NULL;
SemaphoreHandle_t xFilledSlotsSem      = NULL;

void exercicio1(void) {
    ESP_LOGI("ATV1", "Rodando exercicio 1");

    cb_init();

    xCircularBufferMutex = xSemaphoreCreateMutex();
    xEmptySlotsSem       = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    xFilledSlotsSem      = xSemaphoreCreateCounting(BUFFER_SIZE, 0);

    xTaskCreatePinnedToCore(task_read, "READ", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_write, "WRITE", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 1);
}

void exercicio2(void) {
    ESP_LOGI("ATV1", "Rodando exercicio 2");

    cb_init();

    xCircularBufferMutex = xSemaphoreCreateMutex();
    xEmptySlotsSem       = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    xFilledSlotsSem      = xSemaphoreCreateCounting(BUFFER_SIZE, 0);

    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    xTaskCreatePinnedToCore(task_button, "BUTTON", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_log, "LOG", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 1);
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

void cb_read(int *out_data) {
    xSemaphoreTake(xFilledSlotsSem, portMAX_DELAY);

    xSemaphoreTake(xCircularBufferMutex, portMAX_DELAY);
    cb_pop(out_data);
    xSemaphoreGive(xCircularBufferMutex);

    xSemaphoreGive(xEmptySlotsSem);
}

void cb_write(int in_data) {
    xSemaphoreTake(xEmptySlotsSem, portMAX_DELAY);

    xSemaphoreTake(xCircularBufferMutex, portMAX_DELAY);
    cb_push(in_data);
    xSemaphoreGive(xCircularBufferMutex);

    xSemaphoreGive(xFilledSlotsSem);
}

/* Tasks */
void task_read(void *pvParameters) {
    int out_data;
    while (true) {
        cb_read(&out_data);

        ESP_LOGI("Read", "Data read: %d", out_data);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void task_write(void *pvParameters) {
    while (true) {
        for (int i = 0; i < BUFFER_SIZE; i++) {
            cb_write(i);

            ESP_LOGI("Write", "Data written: %d", i);

            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

void task_button(void *pvParameters) {
    int count = 0;
    while (true) {
        if (gpio_get_level(BUTTON_GPIO) == 0) {
            count++;

            cb_write(count);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void task_log(void *pvParameters) {
    int out_count;
    while (true) {
        cb_read(&out_count);

        ESP_LOGI("LOG", "Button pressed %d times", out_count);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
