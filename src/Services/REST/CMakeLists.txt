cmake_minimum_required(VERSION 3.16)

add_library(REST_Client STATIC)
target_sources(REST_Client PRIVATE "PluginRESTService_HttpLib.cpp" "PluginRESTService_Cpr.cpp") 

set_property(TARGET REST_Client PROPERTY CXX_STANDARD 23)

# --- httplib: header-only via FetchContent
include(FetchContent)

FetchContent_Declare(httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.15.0 # Pin to stable version
)

# Configure httplib to use system OpenSSL and disable BoringSSL
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE ON CACHE BOOL "Use OpenSSL if available" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "Disable Brotli" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "Disable zlib" FORCE)

FetchContent_GetProperties(httplib)
if (NOT httplib_POPULATED)
  FetchContent_Populate(httplib)
endif()

FetchContent_MakeAvailable(httplib)

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

# Verify where the header actually is
if (EXISTS "${httplib_SOURCE_DIR}/httplib.h")
  set(_httplib_inc "${httplib_SOURCE_DIR}")
elseif (EXISTS "${httplib_SOURCE_DIR}/include/httplib.h")
  set(_httplib_inc "${httplib_SOURCE_DIR}/include")
else()
  message(FATAL_ERROR "cpp-httplib: couldn't find httplib.h under ${httplib_SOURCE_DIR}")
endif()

# Create an interface target to expose includes & optional flags
add_library(httplib_interface INTERFACE)
target_include_directories(httplib_interface INTERFACE "${_httplib_inc}")

# Check if we have OpenSSL available (from vcpkg or system)
find_package(OpenSSL QUIET)
if(OpenSSL_FOUND) 
  set(HTTPLIB_ENABLE_SSL ON)
  message(STATUS "REST_Client: Using OpenSSL ${OPENSSL_VERSION}")
  target_compile_definitions(httplib_interface INTERFACE CPPHTTPLIB_OPENSSL_SUPPORT)
  target_link_libraries(httplib_interface INTERFACE OpenSSL::SSL OpenSSL::Crypto)
else()
  set(HTTPLIB_ENABLE_SSL OFF)
  message(STATUS "REST_Client: OpenSSL not found - HTTPS support disabled")
 
endif()

# Platform-specific linking
if(WIN32)
	set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client" PREFIX "")
	target_link_libraries(httplib_interface INTERFACE ws2_32) # Windows needs WinSock for sockets
else()
	set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client")
endif()

set_target_properties(REST_Client PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)

message(STATUS "httplib source dir: ${httplib_SOURCE_DIR}")
message(STATUS "Using include: ${_httplib_inc}")
message(STATUS "SSL support: ${HTTPLIB_ENABLE_SSL}")

target_link_libraries(REST_Client PRIVATE 
logger 
httplib_interface 
cpr::cpr)

target_include_directories(REST_Client PRIVATE ${PROJECT_SOURCE_DIR}/include ${_httplib_inc})