/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

int count = 0;
bool on = false;
xSemaphoreHandle sem;

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 5UL )
#define BLINK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )
#define MED_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
#define LOW_TASK_PRIORITY       ( tskIDLE_PRIORITY + 2UL )
#define HIGH_TASK_PRIORITY      ( tskIDLE_PRIORITY + 4UL )

#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BLINK_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

void blink_task(__unused void *params) {
    hard_assert(cyw43_arch_init() == PICO_OK);
    while (true) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
        if (count++ % 11) on = !on;
        vTaskDelay(500);
    }
}

void low_task(__unused void *params) {
    xSemaphoreTake(sem, portMAX_DELAY);
    printf("LOW task took semaphore");
    vTaskSuspend(NULL);    
}

void med_task(__unused void *params) {
}

void high_task(__unused void *params) {
    vTaskDelay((TickType_t)500U);
    while(1) {
        if(uxSemaphoreGetCount(sem)) {
            printf("HIGH task took semaphore");
            xSemaphoreGive(sem);
        }
        else { printf("HIGH task blocked by semaphore"); }
    vTaskDelay((TickType_t)(50U));
    }
}

void main_task(__unused void *params) {
    xTaskCreate(blink_task, "BlinkThread",
                BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    xTaskCreate(low_task, "BlinkThread",
                BLINK_TASK_STACK_SIZE, NULL, LOW_TASK_PRIORITY, NULL);
    xTaskCreate(med_task, "BlinkThread",
                BLINK_TASK_STACK_SIZE, NULL, MED_TASK_PRIORITY, NULL);
    xTaskCreate(high_task, "BlinkThread",
                BLINK_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, NULL);
}

int main( void )
{
    stdio_init_all();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    sem = xSemaphoreCreateBinary();
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread",
                MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);   
    vTaskStartScheduler();
    return 0;
}
