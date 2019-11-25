/*
 * test_queue.cpp
 *
 *  Created on: 11 Feb 2019
 *      Author: efrank
 */

#include <iostream>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using namespace std;

extern "C"
{
#include "nanoQueue.h"

#define SIZE_OF_QUEUE    3

    typedef struct
    {
        uint8_t a[2];
        uint8_t b;
    } test_t;

    nOS_CREATE_TYPED_QUEUE(intiger_queue, int)
    nOS_INSTALL_QUEUE_APIs(intiger_queue, int)

    nOS_CREATE_TYPED_QUEUE(struct_queue, test_t*)
    nOS_INSTALL_QUEUE_APIs(struct_queue, test_t*)

    intiger_queue_t intiger_queue;
    int intiger_queue_buff[SIZE_OF_QUEUE];

    struct_queue_t struct_queue;
    test_t *struct_queue_buff[SIZE_OF_QUEUE];
}

TEST_GROUP(test_queue)
{
    void setup ()
    {
        intiger_queue_init (&intiger_queue, intiger_queue_buff,
                            (SIZE_OF_QUEUE));
        struct_queue_init (&struct_queue, struct_queue_buff, (SIZE_OF_QUEUE));
    }
    void teardown ()
    {

    }
};

TEST(test_queue,test_struct_queue_in_out)
{
    UT_PRINT("test_struct_queue_in_out");
    static test_t struct_in =
    {
    { 1, 2 }, .b = 3 };
    test_t struct_out =
    {
    { 0, 0 }, .b = 0 };

    LONGS_EQUAL(nOS_QUEUE_OK, struct_queue_in (&struct_queue, &struct_in));
    LONGS_EQUAL(nOS_QUEUE_OK, struct_queue_out (&struct_queue, &struct_out));
    LONGS_EQUAL(struct_in.a[0], struct_out.a[0]);
    LONGS_EQUAL(struct_in.a[1], struct_out.a[1]);
    LONGS_EQUAL(struct_in.a[2], struct_out.a[2]);
}

TEST(test_queue,test_queue_in_out)
{
    int num_in = 1, num_out = 0;

    UT_PRINT("test_queue_in_out");

    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_out (&intiger_queue, &num_out));
    LONGS_EQUAL(num_in, num_out);
}

TEST(test_queue,test_queue_size_3_in3_out3)
{
    int num_in = 1, num_out = 0;
    UT_PRINT("test_queue_size_3_in3_out3");

    intiger_queue_in (&intiger_queue, &num_in);
    num_in = 2;
    intiger_queue_in (&intiger_queue, &num_in);
    num_in = 3;
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));

    intiger_queue_out (&intiger_queue, &num_out);
    LONGS_EQUAL(num_out, 1);
    intiger_queue_out (&intiger_queue, &num_out);
    LONGS_EQUAL(num_out, 2);
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_out (&intiger_queue, &num_out));
    LONGS_EQUAL(3, num_out);
}

TEST(test_queue,test_queue_size_3_out_empty_queue)
{
    int num_out = 0;
    UT_PRINT("test_queue_size_3_out_empty_queue");

    intiger_queue_buff[0] = 1;
    intiger_queue_buff[1] = 2;
    intiger_queue_buff[2] = 3;

    LONGS_EQUAL(nOS_QUEUE_EMPTY, intiger_queue_out (&intiger_queue, &num_out));
    LONGS_EQUAL(num_out, 0);
}

TEST(test_queue,test_queue_one_value_in_is_empty_false_one_value_out_is_empty_true)
{
    int num_in = 1, num_out = 0;
    UT_PRINT(
            "test_queue_one_value_in_is_empty_false_one_value_out_is_empty_true");

    intiger_queue_in (&intiger_queue, &num_in);
    CHECK_TRUE(!intiger_queue_is_empty (&intiger_queue));
    intiger_queue_out (&intiger_queue, &num_out);
    CHECK_TRUE(intiger_queue_is_empty (&intiger_queue));
}

TEST(test_queue,test_queue_circular_fill)
{
    int num_in = 0, num_out = 0;
    UT_PRINT("test_queue_circular_fill");

    num_in = 1;
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));

    num_in = 2;
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));

    num_in = 3;
    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));

    LONGS_EQUAL(1, intiger_queue_buff[0]);
    LONGS_EQUAL(2, intiger_queue_buff[1]);
    LONGS_EQUAL(3, intiger_queue_buff[2]);

    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_out (&intiger_queue, &num_out));
    LONGS_EQUAL(1, num_out);

    LONGS_EQUAL(nOS_QUEUE_OK, intiger_queue_in (&intiger_queue, &num_in));
    LONGS_EQUAL(nOS_QUEUE_OVERFLOWED, intiger_queue_in (&intiger_queue, &num_in));
    LONGS_EQUAL(3, intiger_queue_buff[0]);
}

TEST(test_queue, test_queue_tester)
{
    std::cout << std::endl << std::endl
            << "************************ QUEUE GENERATOR TESTER ************************";
}
