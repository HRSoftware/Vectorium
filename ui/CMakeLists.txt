cmake_minimum_required(VERSION 3.15)

project(imgui_glfw)

include(FetchContent)

### -------------------
### Fetch GLFW
### -------------------
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

### -------------------
### Fetch Dear ImGui
### -------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

# Build ImGui core
add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
)

### -------------------
### Build ImGui backend files
### -------------------

find_package(OpenGL REQUIRED)
add_library(imgui_backend STATIC
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backend PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_backend PUBLIC glfw OpenGL::GL)

### -------------------
### Main ImGui GLFW app
### -------------------
add_executable(imgui_glfw
  imgui_glfw.cpp
)

target_include_directories(imgui_glfw PRIVATE
  ${imgui_SOURCE_DIR}
)

target_link_libraries(imgui_glfw
  PRIVATE imgui imgui_backend
)
