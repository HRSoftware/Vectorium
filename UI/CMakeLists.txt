# ui/CMakeLists.txt - CORRECTED VERSION

cmake_minimum_required(VERSION 3.16)

# UI Module - Handles all UI-related functionality

# Fetch UI dependencies
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.4
)
FetchContent_MakeAvailable(imgui)

# ============================================================================
# LIBRARY 1: Pure ImGui backend (no engine dependencies)
add_library(imgui_backend STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui_backend 
	SYSTEM PUBLIC
		${imgui_SOURCE_DIR}
		${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_backend PUBLIC glfw)

# Platform-specific OpenGL linking
if(WIN32)
    target_link_libraries(imgui_backend PUBLIC opengl32)
    target_compile_definitions(imgui_backend PUBLIC IMGUI_IMPL_OPENGL_LOADER_LEGACY)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(imgui_backend PUBLIC OpenGL::GL)
endif()

set_target_properties(imgui_backend PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 23
)

# ============================================================================
# LIBRARY 2: UI services (no engine dependencies, no UI components)
add_library(imgui_service STATIC
    # ONLY UI service implementations - NO UI components here
    "src/Services/UI/PluginUIService_ImGui.cpp"
	"src/Services/UI/PluginUIService_NullObjects.cpp")

target_include_directories(imgui_service 
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/include
	SYSTEM PUBLIC
		${imgui_SOURCE_DIR}
		${imgui_SOURCE_DIR}/backends
)

# FIXED: Only link to what services actually need (no engine!)
target_link_libraries(imgui_service PUBLIC 
    imgui_backend			# Need ImGui for context management
    services_logging        # Need logging

)

set_target_properties(imgui_service PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 23
)

# ============================================================================
# LIBRARY 3: Main UI library (contains UI components, can access engine)
add_library(ui 
	STATIC
		# Main UI components
		src/UI.cpp
		src/EngineUIBridge.cpp
    
		# UI management (these need access to engine)
		src/ImGuiContextManager.cpp
		src/UIServiceManager.cpp
		src/Services/UI/PluginUIService_ImGui.cpp
		src/Services/UI/PluginUIService_NullObjects.cpp)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		${PROJECT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
	SYSTEM PRIVATE
		${imgui_SOURCE_DIR}
		${glfw_SOURCE_DIR}/include
)

# Main UI library can link to engine (this is where engine access happens)
target_link_libraries(ui
    PUBLIC 
		services_core		
		services_logging
        imgui_backend		# For ImGui functionality
        imgui_service		# For UI services
       
    PRIVATE
        engine             # UI components can access engine via interface
        glfw
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(ui PRIVATE opengl32)
    target_compile_definitions(ui PRIVATE IMGUI_IMPL_OPENGL_LOADER_LEGACY)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(ui PRIVATE OpenGL::GL)
endif()

set_target_properties(ui PROPERTIES
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
