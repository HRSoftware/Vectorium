cmake_minimum_required(VERSION 3.16)

add_library(NumberLogger_Plugin SHARED "NumberLogger_Plugin.cpp")


set_property(TARGET NumberLogger_Plugin PROPERTY CXX_STANDARD 23)

if(WIN32)
	set_target_properties(NumberLogger_Plugin PROPERTIES OUTPUT_NAME "NumberLogger_Plugin" PREFIX "")
else()
	set_target_properties(NumberLogger_Plugin PROPERTIES OUTPUT_NAME "NumberLogger_Plugin")
endif()

set_target_properties(NumberLogger_Plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)

target_link_libraries(NumberLogger_Plugin PRIVATE 
	
	services_core
	services_logging)

target_include_directories (NumberLogger_Plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)