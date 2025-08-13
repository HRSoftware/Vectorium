cmake_minimum_required(VERSION 3.16)

add_library(REST_Client STATIC)
target_sources(REST_Client PRIVATE RestClient_HttpLib.cpp)

set_property(TARGET REST_Client PROPERTY CXX_STANDARD 23)

# --- httplib: header-only via FetchContent
include(FetchContent)

FetchContent_Declare(httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  # GIT_TAG v0.15.0 # (optional) pin a known good tag
)

FetchContent_GetProperties(httplib)
if (NOT httplib_POPULATED)
  FetchContent_Populate(httplib)
endif()

FetchContent_MakeAvailable(httplib)

# Verify where the header actually is (root today)
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

# Optional: HTTPS support (needs OpenSSL)
option(HTTPLIB_ENABLE_SSL "Enable HTTPS in cpp-httplib" OFF)

if (HTTPLIB_ENABLE_SSL)
  find_package(OpenSSL REQUIRED)
  target_compile_definitions(httplib_interface INTERFACE CPPHTTPLIB_OPENSSL_SUPPORT)
  target_link_libraries(httplib_interface INTERFACE OpenSSL::SSL OpenSSL::Crypto)
endif()


if(WIN32)
	set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client" PREFIX "")
	target_link_libraries(httplib_interface INTERFACE ws2_32)# Windows needs WinSock for sockets
else()
	set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client")
endif()

set_target_properties(REST_Client PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins # for DLLs
)


message(STATUS "httplib source dir: ${httplib_SOURCE_DIR}")
message(STATUS "Using include: ${_httplib_inc}")

target_link_libraries(REST_Client PRIVATE logger httplib_interface)

target_include_directories (REST_Client PRIVATE ${PROJECT_SOURCE_DIR}/include ${_httplib_inc})