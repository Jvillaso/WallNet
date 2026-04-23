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
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("C:/Users/biote/OneDrive/Documents/WallNet/nrf_code/WallNet_Board_Model/build_2/WallNet_Board_Model/zephyr/cmake/reports/cmake_install.cmake")
endif()

