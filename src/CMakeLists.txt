cmake_minimum_required(VERSION 3.16)

include(${CMAKE_SOURCE_DIR}/cmake/CompilerSettings.cmake)
add_subdirectory(Services)

add_library(engine "Engine.cpp" 
	"PluginInstance.cpp" 
	"PluginManager.cpp"
	"DataPacketRegistry.cpp"
	"Plugin/PluginRuntimeContext.cpp"  
	"Plugin/IPluginContext.cpp"
	"Services/ServiceContainer.cpp")

target_include_directories(engine 
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/include
)

target_compile_features(engine PUBLIC cxx_std_23)
set_target_properties(engine PROPERTIES CXX_STANDARD 23)

target_link_libraries(engine PUBLIC
	services_core
	services_logging
	services_REST
	nlohmann_json::nlohmann_json
)

if (WIN32)
    target_compile_definitions(engine PRIVATE PLATFORM_WINDOWS)
	set_target_properties(engine PROPERTIES
        LINK_FLAGS "/FORCE:UNRESOLVED" )
elseif(UNIX)
    target_compile_definitions(engine PRIVATE PLATFORM_LINUX)
endif()

APPLY_STRICT_COMPILER_SETTINGS(engine)