cmake_minimum_required(VERSION 3.12)
project(bitloom_drivers)

# BITLOOM_CORE must be set to where the bitloom-core repo is located
set(BITLOOM_CORE ${CMAKE_CURRENT_SOURCE_DIR}/../bitloom-core)
set(BITLOOM_DRIVERS ${CMAKE_CURRENT_SOURCE_DIR})

# BITLOOM_CONFIG should be set to the directory containing the configuration
# for the project's target environment as well as the processor specifics.
set(BITLOOM_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/tests)

message("bitloom-core: ${BITLOOM_CORE}")
message("bitloom-drivers: ${BITLOOM_DRIVERS}")
message("bitloom-config: ${BITLOOM_CONFIG}")

add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()
add_subdirectory(src/hmc5883l)
add_subdirectory(tests)
