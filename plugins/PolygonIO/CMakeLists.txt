cmake_minimum_required(VERSION 3.16)

add_library(PolygonIO SHARED "PolygonIO.cpp")

set_property(TARGET PolygonIO PROPERTY CXX_STANDARD 23)


if(WIN32)
	set_target_properties(PolygonIO PROPERTIES OUTPUT_NAME "PolygonIO" PREFIX "")
else()
	set_target_properties(PolygonIO PROPERTIES OUTPUT_NAME "PolygonIO")
endif()

set_target_properties(PolygonIO PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)

# Not currently needed
#target_compile_definitions(PolygonIO PRIVATE VECTORIUM_UI_ENABLED)

target_link_libraries(PolygonIO PRIVATE 
	services_core
	services_logging 
	services_REST
	nlohmann_json::nlohmann_json
	ui
)

target_include_directories (PolygonIO PRIVATE 
	${CMAKE_SOURCE_DIR}/include )
	#${imgui_SOURCE_DIR})  # Add ImGui include directory