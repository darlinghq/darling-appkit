project(AppKit)

cmake_minimum_required(VERSION 2.4.0)

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)


set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -nostdlib -Wl,--version-script=${DARLING_TOP_DIRECTORY}/darwin.map")

add_subdirectory(gui)
add_subdirectory(back)

add_library(AppKit SHARED $<TARGET_OBJECTS:appkit-gui> $<TARGET_OBJECTS:appkit-back>)
target_link_libraries(AppKit CFF system ApplicationServices png12 aspell gif tiff jpeg X11 Xext Xrender fontconfig cairo)
install(TARGETS AppKit DESTINATION ${CMAKE_INSTALL_LIBDIR}/darling)
