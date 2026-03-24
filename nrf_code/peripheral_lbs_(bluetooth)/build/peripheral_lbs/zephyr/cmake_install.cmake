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
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/peripheral_lbs/build/peripheral_lbs/zephyr/cmake/reports/cmake_install.cmake")
endif()

