#pragma once

#include <stdbool.h>
#include <stdio.h>

#include <driver/gpio.h>
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/timers.h>

#define BT_GPIO GPIO_NUM_16

#define TABLE_SIZE 36

static const char *characters[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L",
                                   "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
                                   "Y", "Z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

static const char *morsecode[] = {".-",    "-...",  "-.-.",  "-..",   ".",     "..-.",  "--.",   "....",  "..",
                                  ".---",  "-.-",   ".-..",  "--",    "-.",    "---",   ".--.",  "--.-",  ".-.",
                                  "...",   "-",     "..-",   "...-",  ".--",   "-..-",  "-.--",  "--..",  ".----",
                                  "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", "-----"};

void atividade4(void);
void input_check(TimerHandle_t xTimer);
void print_and_clear_morse(void);
void task_morse_decoder(void *pvParameters);