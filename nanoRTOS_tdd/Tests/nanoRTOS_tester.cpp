/*
 * fake_i2c_ifx_tstr.cpp
 *
 *  Created on: 20 Nov 2019
 *      Author: Ehud Frank
 */

#include <iostream>
#include "string.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C"
{
#include "nanoRTOS.h"
}

typedef struct
{
    uint8_t task1;
    uint8_t task2;
    uint8_t task3;
    uint8_t task4;
    uint8_t task5;
    uint8_t task6;
    uint8_t task7;
    uint8_t task8;
} task_counters_t;

task_counters_t task_counters;

void test_task1 (uint8_t event);
void test_task2 (uint8_t event);
void test_task3 (uint8_t event);
void test_task4 (uint8_t event);
void test_task5 (uint8_t event);
void test_task6 (uint8_t event);
void test_task7 (uint8_t event);
void test_task8 (uint8_t event);

TEST_GROUP(nanoRTOS)
{
    void setup ()
    {
        memset (&task_counters, 0, sizeof(task_counters));
        nOS_start ();
    }
    void teardown ()
    {
        memset (&task_counters, 0, sizeof(task_counters));
    }
};
/**
 * @brief Queue one task per priority and check that all the callback are
 * called by the scheduler
 */
TEST(nanoRTOS, test_nOS_task_in_each_priority_enquue_and_schedule)
{
    UT_PRINT("test_nOS_task_in_each_priority_enquue_and_schedule");
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (1, test_task1, 1));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (2, test_task2, 2));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (3, test_task3, 3));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (4, test_task4, 4));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (5, test_task5, 5));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (6, test_task6, 6));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (7, test_task7, 7));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (8, test_task8, 8));

    CHECK_EQUAL(nOS_OK, nOS_schedule ());

    CHECK_EQUAL(1, task_counters.task1);
    CHECK_EQUAL(1, task_counters.task2);
    CHECK_EQUAL(1, task_counters.task3);
    CHECK_EQUAL(1, task_counters.task4);
    CHECK_EQUAL(1, task_counters.task5);
    CHECK_EQUAL(1, task_counters.task6);
    CHECK_EQUAL(1, task_counters.task7);
    CHECK_EQUAL(1, task_counters.task8);
}
/**
 * Queue 2 tasks in one priority queue and check that the callback is called twice
 */
TEST(nanoRTOS, test_enqueue_task_twice)
{
    UT_PRINT("test_enqueue_task_twice");
    nOS_task_enqueue (1, test_task1, 1);
    nOS_task_enqueue (1, test_task1, 2);
    nOS_schedule ();
    CHECK_EQUAL(2, task_counters.task1);
}
/**
 * Queue a task with priorities out of bound
 */
TEST(nanoRTOS, test_enqueue_check_null_pointer_to_task)
{
    UT_PRINT("test_enqueue_check_null_pointer_to_task");

    CHECK_EQUAL(nOS_TASK_ERR, nOS_task_enqueue (0, 0, 1));
}

/**
 * Queue a task with priorities out of bound
 */
TEST(nanoRTOS, test_enqueue_check_priority_out_of_bounds)
{
    UT_PRINT("test_enqueue_check_priority_out_of_bounds");

    CHECK_EQUAL(nOS_PRIORITY_ERR, nOS_task_enqueue (0, test_task1, 1));
    CHECK_EQUAL(nOS_PRIORITY_ERR, nOS_task_enqueue (9, test_task1, 1));
}

/**
 * Queue a tasks until the queue is full and verify error code
 */
TEST(nanoRTOS, test_enqueue_check_full_task_queue_full)
{
    UT_PRINT("test_enqueue_check_full_task_queue_full");

    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (8, test_task8, 1));
    CHECK_EQUAL(nOS_OK, nOS_task_enqueue (8, test_task8, 1));
    CHECK_EQUAL(nOS_TASK_QUEUE_ERR, nOS_task_enqueue (8, test_task8, 1));
}
TEST(nanoRTOS, nanoRTOS_tester)
{
    std::cout << std::endl << std::endl
            << "************************ nanoRTOS TESTER ************************";
}

void test_task1 (uint8_t event)
{
    task_counters.task1++;
}

void test_task2 (uint8_t event)
{
    task_counters.task2++;
}

void test_task3 (uint8_t event)
{
    task_counters.task3++;
}

void test_task4 (uint8_t event)
{
    task_counters.task4++;
}

void test_task5 (uint8_t event)
{
    task_counters.task5++;
}

void test_task6 (uint8_t event)
{
    task_counters.task6++;
}

void test_task7 (uint8_t event)
{
    task_counters.task7++;
}

void test_task8 (uint8_t event)
{
    task_counters.task8++;
}
