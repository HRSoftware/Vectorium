
cmake_minimum_required(VERSION 3.16)

add_subdirectory(Logger)
add_subdirectory(Mocking)

add_library(engine 
"PluginInstance.cpp" 
"PluginRuntimeContext.cpp" 
"PluginManager.cpp" 
"DataPacketRegistry.cpp" 
"Engine.cpp" 
"Logger/ComponentLogger.cpp")

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(engine PUBLIC cxx_std_23)
set_target_properties(engine PROPERTIES CXX_STANDARD 23)

target_link_libraries(engine PUBLIC logger spdlog::spdlog)

if (WIN32)
    target_compile_definitions(engine PRIVATE PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(engine PRIVATE PLATFORM_LINUX)
endif()