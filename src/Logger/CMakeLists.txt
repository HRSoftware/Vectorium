cmake_minimum_required(VERSION 3.16)

add_library(logger)

set_target_properties(Vectorium PROPERTIES CXX_STANDARD 23)

target_sources(logger PRIVATE ComponentLogger.cpp SpdLogger.cpp LoggablePlugin.cpp)

target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(logger PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(logger PUBLIC spdlog::spdlog_header_only)