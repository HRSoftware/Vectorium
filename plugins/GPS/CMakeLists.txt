cmake_minimum_required(VERSION 3.16)

add_library(GPS_Plugin SHARED "GPS_Plugin.cpp")


set_property(TARGET GPS_Plugin PROPERTY CXX_STANDARD 23)

if(WIN32)
	set_target_properties(GPS_Plugin PROPERTIES OUTPUT_NAME "GPS_Plugin" PREFIX "")
else()
	set_target_properties(GPS_Plugin PROPERTIES OUTPUT_NAME "GPS_Plugin")
endif()

set_target_properties(GPS_Plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)

target_link_libraries(GPS_Plugin PRIVATE logger)

target_include_directories (GPS_Plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)