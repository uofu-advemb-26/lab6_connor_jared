#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "FreeRTOS.h"
#include "task.h"

#define SUPERVISOR_PRIORITY ( tskIDLE_PRIORITY + 10UL )
#define HIGH_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
#define LOW_PRIORITY        ( tskIDLE_PRIORITY + 1UL )

#define RUN_MS          1000
#define RUN_US          ( RUN_MS * 1000 )
#define TOLERANCE_US    50000
static TaskHandle_t thread1, thread2;

void setUp(void) {}
void tearDown(void) {}

void threads_setUp(TaskFunction_t t1, UBaseType_t p1, TaskFunction_t t2, 
           UBaseType_t p2, uint32_t stagger_ms) {

    xTaskCreate(t1, "Thread1", configMINIMAL_STACK_SIZE, NULL, p1, &thread1);
    vTaskDelay(pdMS_TO_TICKS(stagger_ms));
    xTaskCreate(t2, "Thread2", configMINIMAL_STACK_SIZE, NULL, p2, &thread2); 
    vTaskDelay(pdMS_TO_TICKS(RUN_MS - stagger_ms));
}

void threads_tearDown(uint32_t *rt1, uint32_t *rt2) {
    *rt1 = ulTaskGetRunTimeCounter(thread1);
    *rt2 = ulTaskGetRunTimeCounter(thread2);
    vTaskDelete(thread1);
    vTaskDelete(thread2);
}

void busy_busy(__unused void *params)
{
    for (int i = 0; ; i++);
}

void busy_yield(__unused void *params)
{
    for (int i = 0; ; i++) {
        taskYIELD();
    }
}

void test_same_priority_busy_busy(void)
{
    uint32_t runtime1, runtime2;
    threads_setUp(busy_busy, LOW_PRIORITY, busy_busy, LOW_PRIORITY, 0);
    threads_tearDown(&runtime1, &runtime2);

    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, RUN_US / 2, runtime1);
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, RUN_US / 2, runtime2);

}

void test_same_priority_busy_yield(void)
{
    uint32_t runtime1, runtime2;
    threads_setUp(busy_yield, LOW_PRIORITY, busy_yield, LOW_PRIORITY, 0);
    threads_tearDown(&runtime1, &runtime2);

    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, RUN_US / 2, runtime1);
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, RUN_US / 2, runtime2);

}

void test_same_priority_busy_busy_and_yield(void)
{
    uint32_t busy_runtime_us, yield_runtime_us;
    threads_setUp(busy_busy, LOW_PRIORITY, busy_yield, LOW_PRIORITY, 0);
    threads_tearDown(&busy_runtime_us, &yield_runtime_us);

    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, RUN_US, busy_runtime_us);
    TEST_ASSERT_UINT32_WITHIN(TOLERANCE_US, 0, yield_runtime_us);

}

void test_high_priority_busy_busy(void)
{

}

void test_low_priority_busy_busy(void)
{

}

void test_both_priority_busy_yield(void)
{

}


void supervisor_task(__unused void *params)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_same_priority_busy_busy);
        RUN_TEST(test_same_priority_busy_yield);
        RUN_TEST(test_same_priority_busy_busy_and_yield);
        //RUN_TEST(test_high_priority_busy_busy);
        //RUN_TEST(test_low_priority_busy_busy);
        //RUN_TEST(test_both_priority_busy_yield);
        UNITY_END();
    }
}

int main(void)
{
    stdio_init_all();
    xTaskCreate(supervisor_task, "Supervisor", configMINIMAL_STACK_SIZE * 2,
                NULL, SUPERVISOR_PRIORITY, NULL);
    vTaskStartScheduler();
    return 0;
}