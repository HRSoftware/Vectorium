cmake_minimum_required(VERSION 3.16)

add_library(mocking
    MockLogger.cpp
    MockPluginContext.cpp
)

target_include_directories(mocking
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(mocking
    PUBLIC
        logger
        engine # only if mocks depend on engine internals
)