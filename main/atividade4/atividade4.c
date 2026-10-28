#include "atividade4.h"

SemaphoreHandle_t xBtSem;
TimerHandle_t     xMorseTimer;

char input[32]; // input do morse
int  current_pos = 0;

bool button_press = false;

static void IRAM_ATTR bt_isr_handler(void *args) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(xBtSem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE)
        portYIELD_FROM_ISR();
}

void atividade4(void) {
    // Gpio configuration
    gpio_reset_pin(BT_GPIO);
    gpio_set_direction(BT_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BT_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BT_GPIO, GPIO_INTR_NEGEDGE);
    gpio_intr_enable(BT_GPIO);

    // Isr
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BT_GPIO, bt_isr_handler, NULL);

    // Timer
    xMorseTimer = xTimerCreate("MORSE", pdMS_TO_TICKS(800), pdFALSE, NULL, input_check);

    // Semaphore
    xBtSem = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(task_morse_decoder, "MORSE", 2048, NULL, 10, NULL, 0);
}

// Roda o timer após pressionar o botão - Ao final checa o estado do botão
void input_check(TimerHandle_t xTimer) {
    // Botão ainda pressionado = traço
    if (gpio_get_level(BT_GPIO) == 0)
        input[current_pos] = '-';
    // Botão não pressionado = ponto
    else
        input[current_pos] = '.';

    ESP_LOGI("MORSE", "%s", input);
    current_pos++;
}

// Roda após o botão não estar apertado por 3 segundos
void print_and_clear_morse(void) {
    input[current_pos] = '\0';

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (strcmp(input, morsecode[i]) == 0) {
            ESP_LOGI("MORSE", "Morse identificado: %s = %s", morsecode[i], characters[i]);
            memset(input, 0, sizeof(input));
            current_pos = 0;
            return;
        }
    }

    ESP_LOGI("MORSE", "Morse não identificado: %s", input);
    memset(input, 0, sizeof(input));
    current_pos = 0;
}

void task_morse_decoder(void *pvParameters) {
    BaseType_t ret;
    while (true) {
        // Espera o clique do botão e ativa o timer de verificação
        ret = xSemaphoreTake(xBtSem, pdMS_TO_TICKS(2000));
        if (ret == pdFALSE && current_pos > 0) {
            print_and_clear_morse();
            continue;
        }
        if (ret == pdTRUE)
            xTimerStart(xMorseTimer, 0);
    }
}