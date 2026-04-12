cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(WallNet_1)

zephyr_include_directories(inc)

target_sources(app PRIVATE 
    src/main.c
    src/wallnet_ble.c
    src/wallnet_ui.c
    src/fp_commands.c
    src/fp_commands.h
    src/wallnet_gps.c
)
