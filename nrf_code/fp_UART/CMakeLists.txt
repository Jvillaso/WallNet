cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gps_trying)

# debugging setup, sets whether we have straight UART outptu or parsed GPS data output
target_sources(app PRIVATE 
src/main.c
src/fp_commands.c
src/fp_commands.h
)