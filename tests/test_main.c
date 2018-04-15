#include "unity_fixture.h"

static void run_tests(void)
{
    RUN_TEST_GROUP(ssd1306);
}

int main(int argc, const char* argv[])
{
    return UnityMain(argc, argv, run_tests);
}
