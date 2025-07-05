cmake_minimum_required(VERSION 3.16)

set_property(TARGET Vectorium PROPERTY CXX_STANDARD 23)
# Fetch external libraries locally (only inside ui)
include(FetchContent)

find_package(OpenGL REQUIRED)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)

# Disable building extra stuff
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

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