cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gps_trying)

# debugging setup, sets whether we have straight UART outptu or parsed GPS data output
if(CONFIG_GPS_PARSED_OUTPUT)
    target_sources(app PRIVATE src/main_parsed.c)
else()
    target_sources(app PRIVATE src/main_nmea.c)
endif()