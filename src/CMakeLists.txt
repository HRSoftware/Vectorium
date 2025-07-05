
cmake_minimum_required (VERSION 3.12)

add_library(engine 

"LoadedPlugin.cpp" 
"PluginContextImpl.cpp" 
"PluginManager.cpp" 
"DataPacketRegistry.cpp" 
"Engine.cpp")

target_include_directories(engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(engine PUBLIC cxx_std_20)
set_target_properties(engine PROPERTIES CXX_STANDARD 23)

if (WIN32)
    target_compile_definitions(engine PRIVATE PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(engine PRIVATE PLATFORM_LINUX)
endif()