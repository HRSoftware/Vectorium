cmake_minimum_required(VERSION 3.16)

# Replace imgui_backend with imgui_static
add_library(ui
    "src/UI.cpp"
    "src/EngineUIBridge.cpp")

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/include
        ${imgui_SOURCE_DIR}
        ${glfw_SOURCE_DIR}/include
)

target_link_libraries(ui
    PRIVATE
        engine
        glfw
        imgui_static  # Single shared ImGui library
        OpenGL::GL
        logger
)