cmake_minimum_required(VERSION 3.16)

add_library(logger)

target_sources(logger PRIVATE ComponentLogger.cpp SpdLogger.cpp LoggablePlugin.cpp)

target_include_directories(logger PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(logger PUBLIC spdlog::spdlog)