cmake_minimum_required(VERSION 3.0)
project(TinyCC
	VERSION 0.9.27
	# DESCRIPTION "Tiny C Compiler" # cmake version >= 3.9
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "Current OS is ${CMAKE_SYSTEM}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # must before add_executable

add_subdirectory(libtcc)
add_subdirectory(tcc)
add_subdirectory(demo)

add_custom_target(CopyLibtcc ALL)
add_dependencies(CopyLibtcc libtcc)
add_custom_command(TARGET CopyLibtcc POST_BUILD
	# COMMAND ${CMAKE_COMMAND} -E echo "target dir = $<TARGET_FILE:libtcc> $<TARGET_FILE_DIR:tcc> $<TARGET_FILE:tcc>"
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:libtcc>" "$<TARGET_FILE_DIR:tcc>"
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:libtcc>" "$<TARGET_FILE_DIR:libtcc-usage>"
)

if(WIN32)
	set(CC "$<TARGET_FILE:tcc>" CACHE STRING "path of tcc")
	add_subdirectory(runtime)
	add_dependencies(libtcc1 CopyLibtcc)
endif(WIN32)
