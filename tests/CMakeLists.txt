message("CPPUTEST_HOME: '${CPPUTEST_HOME}'")
find_library(CPPUTESTLIB NAMES CppUTest HINTS ${CPPUTEST_HOME}/cpputest_build/lib)
find_library(CPPUTESTEXTLIB NAMES CppUTestExt HINTS ${CPPUTEST_HOME}/cpputest_build/lib)
message("CppUTest lib: ${CPPUTESTLIB}")
message("CppUTestExt lib: ${CPPUTESTEXTLIB}")

add_executable(hmc5883l_test
    hmc5883l/Hmc5883lTest.cpp
    mocks/i2c_mock.cpp
    )

#target_include_directories(hmc5883l_test PRIVATE ${CPPUTEST_HOME}/include)
target_include_directories(hmc5883l_test PRIVATE ${BITLOOM_DRIVERS}/include)
target_include_directories(hmc5883l_test PRIVATE ${BITLOOM_CORE}/include)
target_include_directories(hmc5883l_test PRIVATE ${BITLOOM_CONFIG})
target_include_directories(hmc5883l_test PRIVATE mocks)

target_link_libraries(hmc5883l_test
    hmc5883l
    ${CPPUTESTLIB}
    ${CPPUTESTEXTLIB}
    )
