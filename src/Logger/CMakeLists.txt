cmake_minimum_required(VERSION 3.16)

add_library(logger "SpdLogger.cpp" )

set_target_properties(logger PROPERTIES CXX_STANDARD 23)

target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include)

set_target_properties(logger PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(logger PUBLIC spdlog::spdlog_header_only)