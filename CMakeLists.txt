cmake_minimum_required (VERSION 3.16)

include(${CMAKE_SOURCE_DIR}/cmake/CompilerSettings.cmake)

# Affect all projects onwards
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Try to find OpenSSL through vcpkg first
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND)
    message(STATUS "Found OpenSSL via vcpkg: ${OPENSSL_VERSION}")
    set(HAVE_OPENSSL TRUE)
else()
    message(STATUS "OpenSSL not found via vcpkg - SSL features will be disabled")
    set(HAVE_OPENSSL FALSE)
endif()


##	nlohmannJson
FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)



# Add source to this project's executable.
add_executable (Vectorium "Vectorium.cpp")



target_link_libraries(Vectorium PRIVATE engine ui)

add_subdirectory(plugins)
add_subdirectory(UI)
add_subdirectory(src)


APPLY_STRICT_COMPILER_SETTINGS(Vectorium)