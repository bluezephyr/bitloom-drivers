[![Build Status](https://travis-ci.org/bluezephyr/bitloom-drivers.svg?branch=master)](https://travis-ci.org/bluezephyr/bitloom-drivers)

# BitLoom Drivers

This repository contains drivers for hardware to be used in products based on BitLoom.

## SSD1306

BitLoom driver for the SSD1306 OLED display.

## HMC5883L

BitLoom driver for the HMC5883L compass.

## Build Toolchain

BitLoom is built using CMake. For the BitLoom drivers, the following variable must be set:

* BITLOOM_CORE -- Path to the [bitloom-core](https://github.com/bluezephyr/bitloom-cor) git
  repository. The variable shall be relative to the root of the bitloom-drivers repo.

If the bitloom-core repository is located in the same folder as bitloom-drivers, the variable
can be set when cmake is invoked. `cmake -DBITLOOM_CORE=<path to bitloom-core>` See the unit
test section below for more information.

## Unit Tests
The project includes a set of unit tests. The tests use the [CppUTest](http://cpputest.github.io/)
test framework.

On Linux use the following sequence to run the unit tests.

    git clone https://github.com/bluezephyr/bitloom-core.git
    git clone https://github.com/bluezephyr/bitloom-drivers.git
    mkdir bitloom-drivers/build
    cd bitloom-drivers/build
    cmake -DBITLOOM_CORE=../bitloom-core ..
    cd bitloom-drivers/tests
    make all test

Unit tests are executed on each commit by Travis CI:
https://travis-ci.org/bluezephyr/bitloom-drivers
