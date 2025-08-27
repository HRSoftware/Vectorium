cmake_minimum_required(VERSION 3.16)

add_library(NumberGenerator_Plugin SHARED "NumberGenerator_Plugin.cpp")


set_property(TARGET NumberGenerator_Plugin PROPERTY CXX_STANDARD 23)

if(WIN32)
	set_target_properties(NumberGenerator_Plugin PROPERTIES OUTPUT_NAME "NumberGenerator_Plugin" PREFIX "")
else()
	set_target_properties(NumberGenerator_Plugin PROPERTIES OUTPUT_NAME "NumberGenerator_Plugin")
endif()

set_target_properties(NumberGenerator_Plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)

target_link_libraries(NumberGenerator_Plugin PRIVATE 
	services_core 
	services_logging services_REST)

target_include_directories (NumberGenerator_Plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)