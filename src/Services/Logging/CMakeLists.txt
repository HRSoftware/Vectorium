cmake_minimum_required(VERSION 3.16)

# Fetch spdlog here since this is where it's used
include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)

# Set options before making available
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spdlog)

# Create the logger library
add_library(services_logging STATIC 
    "SpdLogger.cpp"  
    "PluginLogger.cpp"
    "UILogSink.cpp")

set_target_properties(services_logging PROPERTIES 
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
)

target_include_directories(services_logging 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
        ${spdlog_SOURCE_DIR}/include  # Explicit spdlog include
)

# Link against spdlog
target_link_libraries(services_logging PUBLIC
	services_core
    spdlog
		
)

# Set output directories
set_target_properties(services_logging PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Debug output
message(STATUS "spdlog_SOURCE_DIR: ${spdlog_SOURCE_DIR}")