
cmake_minimum_required(VERSION 3.16)

add_subdirectory(Logger)
add_subdirectory(Mocking)
add_subdirectory(RestClient)

add_library(engine "Engine.cpp" 
"PluginInstance.cpp" 
"PluginManager.cpp"
"DataPacketRegistry.cpp"
"Plugin/PluginRuntimeContext.cpp"  
"Plugin/IPluginContext.cpp" )

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(engine PUBLIC cxx_std_23)
set_target_properties(engine PROPERTIES CXX_STANDARD 23)

target_link_libraries(engine PUBLIC 
logger 
nlohmann_json::nlohmann_json 
REST_Client)

if (WIN32)
    target_compile_definitions(engine PRIVATE PLATFORM_WINDOWS)
	set_target_properties(engine PROPERTIES
        LINK_FLAGS "/FORCE:UNRESOLVED" )
elseif(UNIX)
    target_compile_definitions(engine PRIVATE PLATFORM_LINUX)
endif()