enable_language(C)
set(CMAKE_C_STANDARD 99)

add_library(hmc5883l
    hmc5883l.c
    )

target_include_directories(hmc5883l PUBLIC ${BITLOOM_DRIVERS}/include)
target_include_directories(hmc5883l PRIVATE ${BITLOOM_CORE}/include)
target_include_directories(hmc5883l PRIVATE ${BITLOOM_CONFIG})
