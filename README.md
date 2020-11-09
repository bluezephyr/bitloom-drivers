[![Build Status](https://travis-ci.org/bluezephyr/bitloom-drivers.svg?branch=master)](https://travis-ci.org/bluezephyr/bitloom-drivers)

# BitLoom Drivers
This repository contains drivers for hardware to be used in products based on
Bitloom.

## SSD1306
Bitloom driver for the SSD1306 chip

## Unit Tests
The project includes a set of unit tests. The tests use the Unity
(http://www.throwtheswitch.org/unity) test framework. Make sure to download the
test framework from https://github.com/ThrowTheSwitch/Unity and set the
environment variable UNITY_HOME.

On Linux use the following sequence:

    export UNITY_HOME=<path to Unity folder>
    cd bitloom-drivers/tests
    make

Unit tests are executed on each commit by Travis CI:
https://travis-ci.org/bluezephyr/bitloom-drivers
