cmake_minimum_required (VERSION 3.16)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Vectorium")

# Check if vcpkg toolchain is available
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using vcpkg toolchain: ${CMAKE_TOOLCHAIN_FILE}")
else()
    message(WARNING "No vcpkg toolchain detected. Consider setting CMAKE_TOOLCHAIN_FILE to vcpkg.cmake")
endif()

#################################
## Package Finding (vcpkg first, then FetchContent fallback)
################################
include(FetchContent)

find_package(OpenGL REQUIRED)

# Try to find OpenSSL through vcpkg first
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    message(STATUS "Found OpenSSL via vcpkg: ${OPENSSL_VERSION}")
    set(HAVE_OPENSSL TRUE)
else()
    message(STATUS "OpenSSL not found via vcpkg - SSL features will be disabled")
    set(HAVE_OPENSSL FALSE)
endif()

###			 GLFW
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


###			ImGui static build
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.9
)
FetchContent_MakeAvailable(imgui)

# Create ImGui as a static library instead of static
add_library(imgui_static STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_static PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_static PUBLIC glfw OpenGL::GL)



set_target_properties(imgui_static PROPERTIES 
    OUTPUT_NAME "imgui"
    CXX_STANDARD 23
)

###			SPDLog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

##	nlohmannJson
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

### cpr (C++ Requests)
FetchContent_Declare(
  cpr 
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)

# Configure cpr options
set(CPR_USE_SYSTEM_CURL OFF CACHE BOOL "Use system curl")
set(CPR_BUILD_TESTS OFF CACHE BOOL "Don't build cpr tests")
set(CPR_ENABLE_SSL ON CACHE BOOL "Enable SSL support")

FetchContent_MakeAvailable(cpr)

# Add source to this project's executable.
add_executable (Vectorium "Vectorium.cpp")

set_target_properties(Vectorium PROPERTIES CXX_STANDARD 23)

target_link_libraries(Vectorium PRIVATE engine ui)

add_subdirectory(plugins)
add_subdirectory(ui)
add_subdirectory(src)