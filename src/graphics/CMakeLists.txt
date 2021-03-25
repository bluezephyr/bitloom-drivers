add_library(graphics
    framebuffer.c
    graphics.c
    )

target_include_directories(graphics PUBLIC ${BITLOOM_DRIVERS}/include)
target_include_directories(graphics PRIVATE ${BITLOOM_CORE}/include)
target_include_directories(graphics PRIVATE ${BITLOOM_CONFIG})
