#
#  GattLib++ - GATT Library C++ Wrapper
#
#  Copyright (C) 2017  Pat Deegan, https://psychogenic.com
#
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

cmake_minimum_required(VERSION 2.6)


project(gattlibpp)

find_package(PkgConfig REQUIRED)

# Show all the warnings
set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} -Wall")
include_directories(include)

pkg_search_module(GATTLIB REQUIRED gattlib)


add_subdirectory(gattlibpp)
# Add the Gattlibpp build directory for the examples
link_directories(${PROJECT_BINARY_DIR}/gattlibpp)


#
# Packaging
#
set(CPACK_PACKAGE_INSTALL_DIRECTORY /usr CACHE STRING "Install directory (default: /usr).")
if (ENV{TRAVIS_TAG})
  set(CPACK_PACKAGE_VERSION $ENV{TRAVIS_TAG})
else()
  set(CPACK_PACKAGE_VERSION 1.0.2)
endif()
set(CPACK_PACKAGE_CONTACT "Pat Deegan, https://psychogenic.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Wrapper for gattlib, to access GATT information from Bluetooth Low Energy (BLE) devices")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${CPACK_PACKAGE_VERSION}_${CPACK_PACKAGE_ARCHITECTURE}")







# Generate pkg-config file before building the examples
configure_file(gattlibpp/gattlibpp.pc.in ${PROJECT_BINARY_DIR}/gattlibpp.pc @ONLY)

# Add the build directory to PKG_CONFIG_PATH
set(ENV{PKG_CONFIG_PATH} "${PROJECT_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}")

# Examples
add_subdirectory(examples/ble_scan)
add_subdirectory(examples/read_write)
#add_subdirectory(examples/notification)
#add_subdirectory(examples/nordic_uart)

#
# Debian, RPM and ZIP packages
#
if (CMAKE_SYSROOT)
  # CPack does like RPM package in a cross-toolchain context
  set(CPACK_GENERATOR "DEB;ZIP")
else()
  set(CPACK_GENERATOR "DEB;RPM;ZIP")
endif()

if (NOT CMAKE_SYSROOT)
# Detect platform architecture to use it for the Debian package
  execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_QUIET)
endif()

set(CPACK_DEBIAN_PACKAGE_DEPENDS "gattlib (>= 0.3), libglib2.0-0 (>= 2.54.0), bluez (>= 5.40)")
#
# List of file to install
#

install(FILES include/Gattlibpp.h DESTINATION include)
install(FILES 
	include/gattlibpp/BLECentralConfig.h
	include/gattlibpp/BLECentralDebug.h
	include/gattlibpp/BLECentral.h
	include/gattlibpp/DeviceDetails.h
	include/gattlibpp/GattlibppExtInc.h
	include/gattlibpp/GattlibppPlatform.h
	include/gattlibpp/GattlibppTypes.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/BLECentralDebug.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/BLECentral.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/DeviceDetails.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/GattlibppExtInc.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/GattlibppPlatform.h DESTINATION include/gattlibpp)
#install(FILES include/gattlibpp/GattlibppTypes.h DESTINATION include/gattlibpp)

install(FILES ${PROJECT_BINARY_DIR}/gattlibpp.pc DESTINATION lib/pkgconfig)

include(CPack)
