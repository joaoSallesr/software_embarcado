#include "atividade2.h"

#define QUEUE_LENGTH 20
#define DELAY_IN_MS  200

QueueHandle_t xTempQueue  = NULL;
QueueHandle_t xHumidQueue = NULL;
QueueHandle_t xMeanQueue  = NULL;

void atv2_ex1(void) {
    ESP_LOGI("ATV2", "Rodando exercicio 1");

    xHumidQueue = xQueueCreate(QUEUE_LENGTH, sizeof(float));
    xTempQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(float));
    xMeanQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(data_t));

    xTaskCreatePinnedToCore(task_log, "LOG", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_humid_mean, "H_MEAN", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_temp_mean, "T_MEAN", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(task_read, "READ", configMINIMAL_STACK_SIZE * 8, NULL, 10, NULL, 1);
}

void task_read(void *pvParameters) {
    float humidity, temperature;

    while (true) {
        esp_err_t ret = dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO, &humidity, &temperature);
        if (ret == ESP_OK) {
            xQueueSend(xHumidQueue, &humidity, portMAX_DELAY);
            xQueueSend(xTempQueue, &temperature, portMAX_DELAY);
        } else
            ESP_LOGE("READ", "Error reading sensor.");

        vTaskDelay(pdMS_TO_TICKS(DELAY_IN_MS));
    }
}

void task_humid_mean(void *pvParameters) {
    float humid      = 0;
    float humid_mean = 0;

    uint16_t humid_count = 0;

    while (true) {
        xQueueReceive(xHumidQueue, &humid, portMAX_DELAY);
        humid_count++;
        humid_mean += humid;

        if (humid_count == 10) {
            humid_mean /= 10;

            data_t mean = {
                .humidity    = humid_mean,
                .temperature = 0,
            };

            xQueueSend(xMeanQueue, &mean, portMAX_DELAY);

            humid_mean  = 0;
            humid_count = 0;
        }
    }
}

void task_temp_mean(void *pvParameters) {
    float temp      = 0;
    float temp_mean = 0;

    uint16_t temp_count = 0;

    while (true) {
        xQueueReceive(xTempQueue, &temp, portMAX_DELAY);
        temp_count++;
        temp_mean += temp;

        if (temp_count == 10) {
            temp_mean /= 10;

            data_t mean = {
                .humidity    = 0,
                .temperature = temp_mean,
            };

            xQueueSend(xMeanQueue, &mean, portMAX_DELAY);

            temp_mean  = 0;
            temp_count = 0;
        }
    }
}

void task_log(void *pvParameters) {
    data_t mean;

    while (true) {
        xQueueReceive(xMeanQueue, &mean, portMAX_DELAY);
        if (mean.humidity != 0) {
            ESP_LOGI("LOG", "Umidade (10): %f", mean.humidity);
        } else if (mean.temperature != 0) {
            ESP_LOGI("LOG", "Temperatura (10): %f", mean.temperature);
        }
    }
}