#include "aula14.h"

SemaphoreHandle_t xBtSem;

static void IRAM_ATTR bt_isr_handler(void *args) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(xBtSem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR();
}

void aula14(void) {
    // Hardware Configuration
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_GPIO, 1);

    gpio_reset_pin(BT_GPIO);
    gpio_set_direction(BT_GPIO, GPIO_MODE_INPUT);

    gpio_set_intr_type(BT_GPIO, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(BT_GPIO);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BT_GPIO, bt_isr_handler, NULL);

    xBtSem = xSemaphoreCreateBinary();

    xTaskCreate(task_led, "LED", 2048, NULL, 2, NULL);
}

void task_led(void *pvParameters) {
    uint16_t cnt = 0;
    while (true) {
        xSemaphoreTake(xBtSem, portMAX_DELAY);
        cnt++;

        ESP_LOGI("LED", "Button pressed - CNT = %d", cnt);
        gpio_set_level(LED_GPIO, cnt % 2);
    }
}
