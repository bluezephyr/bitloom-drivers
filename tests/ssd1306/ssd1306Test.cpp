/*
 * Unit tests for the SSD1306 BitLoom driver.
 *
 * Copyright (c) 2021. BlueZephyr
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 *
 */
#include <CppUTest/CommandLineTestRunner.h>
#include <CppUTestExt/MockSupport.h>

extern "C"
{
    #include "ssd1306.h"
}


TEST_GROUP(ssd1306)
{
    void setup() override
    {
    }

    void teardown() override
    {
        mock().checkExpectations();
        mock().clear();
    }
};

/********************************************************************
 * TEST CASES
 ********************************************************************/
TEST(ssd1306, init)
{
    FAIL("Init");
}

/********************************************************************
 * TEST RUNNER
 ********************************************************************/
int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
