#include "aula17.h"

TimerHandle_t     timer_blink;
TimerHandle_t     timer_ledoff;
SemaphoreHandle_t xBtSem;

static void IRAM_ATTR bt_isr_handler(void *args) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(xBtSem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR();
}

void aula17(void) {
    // Configuração GPIOs
    gpio_reset_pin(BT_GPIO);
    gpio_set_direction(BT_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BT_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BT_GPIO, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(BT_GPIO);

    gpio_reset_pin(LED1_GPIO);
    gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED1_GPIO, 1);

    gpio_reset_pin(LED2_GPIO);
    gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(LED2_GPIO, 1);

    // Configuração Timers
    timer_blink = xTimerCreate("BLINK", pdMS_TO_TICKS(500), pdTRUE, NULL, led_blink);
    xTimerStart(timer_blink, 0);

    timer_ledoff = xTimerCreate("OFF", pdMS_TO_TICKS(5000), pdTRUE, NULL, led_off);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BT_GPIO, bt_isr_handler, NULL);

    xBtSem = xSemaphoreCreateBinary();

    xTaskCreate(task_button, "BT", 2048, NULL, 5, NULL);
}

void led_blink(TimerHandle_t xTimer) {
    static uint16_t cnt = 0;
    gpio_set_level(LED1_GPIO, (cnt++) % 2);
}

void task_button(void *pvParameters) {
    while (true) {
        xSemaphoreTake(xBtSem, portMAX_DELAY);

        ESP_LOGI("LED", "Button pressed");
        gpio_set_level(LED2_GPIO, 0);
        xTimerStart(timer_ledoff, 0);
    }
}

void led_off(TimerHandle_t xTimer) { gpio_set_level(LED2_GPIO, 1); }