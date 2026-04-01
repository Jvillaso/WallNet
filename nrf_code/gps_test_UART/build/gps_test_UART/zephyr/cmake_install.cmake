# Install script for directory: C:/ncs/v3.2.2/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Zephyr-Kernel")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/ncs/toolchains/c717907b94/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/gps_test_UART/build/gps_test_UART/zephyr/cmake/reports/cmake_install.cmake")
endif()

