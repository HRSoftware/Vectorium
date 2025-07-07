cmake_minimum_required(VERSION 3.16)

add_library(imgui_backend
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backend PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_backend PUBLIC glfw OpenGL::GL)

# UI library
add_library(ui
    UI.cpp
    EngineUIBridge.cpp
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/include
		${imgui_SOURCE_DIR}
	    ${glfw_SOURCE_DIR}/include
)

target_link_libraries(ui
    PRIVATE
        engine
        glfw
		imgui_backend
        OpenGL::GL
)