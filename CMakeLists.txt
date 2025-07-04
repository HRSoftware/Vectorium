# CMakeList.txt : CMake project for Vectorium, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.12)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Vectorium")


# Add source to this project's executable.
add_executable (Vectorium "Vectorium.cpp" "Vectorium.h" "include/Core/IDataHandler.h" "include/Core/IPlugin.h" "include/Core/PluginLoading.h" "src/LoadedPlugin.cpp" "include/Core/PluginContextImpl.h" "src/PluginLoading.cpp" "src/PluginContextImpl.cpp")


set_property(TARGET Vectorium PROPERTY CXX_STANDARD 23)


add_subdirectory(plugins/GPS)
add_subdirectory(ui)


# TODO: Add tests and install targets if needed.
