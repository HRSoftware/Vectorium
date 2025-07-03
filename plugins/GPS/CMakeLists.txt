cmake_minimum_required(VERSION 3.8)

add_library (GPS_Plugin SHARED GPS_Plugin.cpp)


target_include_directories (GPS_Plugin PRIVATE ${CMAKE_SOURCE_DIR}/include/Core) # IPlugin.h etc
set_property(TARGET GPS_Plugin PROPERTY CXX_STANDARD 23)
set_target_properties(GPS_Plugin PROPERTIES OUTPUT_NAME "GPS_Plugin" PREFIX "lib")