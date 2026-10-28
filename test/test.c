#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"

void setUp(void) {}

void tearDown(void) {}

void thread_pair_setup(t1, t2){
    xTaskCreate(t1, "Thread1", BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    xTaskCreate(t2, "Thread2", BLINK_TASK_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    vTaskStartScheduler();
}

void test_same_priority_busy_busy(void)
{
    thread_pair_setup(busy_busy, busy_busy);
    UNITY_TEST_ASSERT 

}

void test_same_priority_busy_yield(void)
{

}

void test_same_priority_busy_busy_and_yield(void)
{

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


int main (void)
{
    stdio_init_all();
    while (1) {
        sleep_ms(5000); // Give time for TTY to attach.
        printf("Start tests\n");
        UNITY_BEGIN();
        RUN_TEST(test_same_priority_busy_busy);
        RUN_TEST(test_same_priority_busy_yield);
        RUN_TEST(test_same_priority_busy_busy_and_yield);
        RUN_TEST(test_high_priority_busy_busy);
        RUN_TEST(test_low_priority_busy_busy);
        RUN_TEST(test_both_priority_busy_yield);
        sleep_ms(5000);
        UNITY_END();
    }
}
