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

# Apply compiler settings to suppress warnings in spdlog
if(MSVC AND TARGET spdlog)
    target_compile_definitions(spdlog PRIVATE
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING=1
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
    )
    
    target_compile_options(spdlog PRIVATE /w)  # Also disable all warnings for spdlog
endif()

# Create the logger library
add_library(services_logging STATIC 
    "SpdLogger.cpp"  
    "PluginLogger.cpp"
    "UILogSink.cpp")

set_target_properties(services_logging 
	PROPERTIES 
		CXX_STANDARD 23
		POSITION_INDEPENDENT_CODE ON
)

target_include_directories(services_logging 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
	SYSTEM PUBLIC
        ${spdlog_SOURCE_DIR}/include  # Explicit spdlog include
)

# Link against spdlog
target_link_libraries(services_logging 
	PUBLIC
		services_core
	PUBLIC
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