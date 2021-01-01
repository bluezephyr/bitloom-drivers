enable_language(C)
set(CMAKE_C_STANDARD 99)

add_library(ssd1306
    ssd1306.c
    ssd1306_defines.h)

target_include_directories(ssd1306 PUBLIC ${BITLOOM_DRIVERS}/include)
target_include_directories(ssd1306 PRIVATE ${BITLOOM_CORE}/include)
target_include_directories(ssd1306 PRIVATE ${BITLOOM_CONFIG})
