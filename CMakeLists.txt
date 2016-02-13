project(AppKit)

cmake_minimum_required(VERSION 2.4.0)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(Qt5Declarative REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)

#if (NOT "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" MATCHES ".*clang")
#	message(FATAL_ERROR "Clang is the only supported compiler.")
#endif (NOT "${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1}" MATCHES ".*clang")

#configure_file(config.h.in config.h)
add_definitions(-D__STDC_LIMIT_MACROS)
add_definitions(-DDARLING_BUILD)
add_definitions(-DOBJC2RUNTIME -U__APPLE__)
add_definitions(-DHAVE_STDINT_H=1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fblocks -fconstant-string-class=NSConstantString -fblocks -fobjc-runtime=gnustep-1.7")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,--version-script=${DARLING_TOP_DIRECTORY}/darwin.map")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${DARLING_TOP_DIRECTORY}/src/util)
include_directories(${CMAKE_BINARY_DIR}/src/external/corefoundation/Headers)
include_directories(${CMAKE_BINARY_DIR}/src/external/foundation/Headers)
include_directories(${DARLING_TOP_DIRECTORY}/src/external/foundation/Headers)
include_directories(${DARLING_TOP_DIRECTORY}/src/external/corefoundation/Headers)
include_directories(${DARLING_TOP_DIRECTORY}/src/external/libobjc2)
include_directories(${DARLING_TOP_DIRECTORY}/src/external/libdispatch)

set(AppKit_SRCS
	NSApplication.mm
	NSEvent.mm
	NSResponder.mm
	NSView.mm
	NSWindow.mm
	Util.mm
	QNSEventDispatcher.cpp
        QWindowSubclass.mm
)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/darling")
#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(AppKit SHARED ${AppKit_SRCS})
target_link_libraries(AppKit CFFExtra)
qt5_use_modules(AppKit Declarative Widgets Quick)

install(TARGETS AppKit DESTINATION "${CMAKE_INSTALL_LIBDIR}/darling")

