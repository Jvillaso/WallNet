# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF")
  file(MAKE_DIRECTORY "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF")
endif()
file(MAKE_DIRECTORY
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/rachel_NRF"
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix"
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/tmp"
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/src/rachel_NRF-stamp"
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/src"
  "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/src/rachel_NRF-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/src/rachel_NRF-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/rachelchen/Projects/WallNet/nrf_code/rachel_NRF/build/_sysbuild/sysbuild/images/rachel_NRF-prefix/src/rachel_NRF-stamp${cfgdir}") # cfgdir has leading slash
endif()
