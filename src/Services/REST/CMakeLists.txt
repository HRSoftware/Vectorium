cmake_minimum_required(VERSION 3.16)

add_library(REST_Client STATIC)


set_target_properties(REST_Client PROPERTIES 
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
)

set(USE_HTTPLIB OFF CACHE BOOL "Use cpp-httplib as REST client backend. Fall back to CPR otherwise" FORCE)


target_sources(REST_Client PRIVATE "PluginRESTServiceFactory.cpp")

# === cpp-httplib Implementation ===
if(USE_HTTPLIB)
    message(STATUS "Using cpp-httplib REST client implementation")

	set(USE_CPR OFF)
    
	target_compile_definitions(REST_Client PRIVATE USE_HTTPLIB)

    # Add implementation source
    target_sources(REST_Client PRIVATE PluginRESTService_HttpLib.cpp)
    
    # Fetch cpp-httplib dependency
    include(FetchContent)
    FetchContent_Declare(httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG v0.15.0
    )
    
    # Configure httplib options
    set(HTTPLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(HTTPLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(httplib)
    
    # Create interface target for httplib
    add_library(httplib_interface INTERFACE)
    target_include_directories(httplib_interface INTERFACE "${httplib_SOURCE_DIR}")
    
    # Optional: HTTPS support
    option(HTTPLIB_ENABLE_SSL "Enable HTTPS support in cpp-httplib" OFF)
    if(HTTPLIB_ENABLE_SSL)
        find_package(OpenSSL REQUIRED)
        target_compile_definitions(httplib_interface INTERFACE CPPHTTPLIB_OPENSSL_SUPPORT)
        target_link_libraries(httplib_interface INTERFACE OpenSSL::SSL OpenSSL::Crypto)
        message(STATUS "cpp-httplib: HTTPS support enabled")
    endif()
    
    # Platform-specific libraries
    if(WIN32)
        target_link_libraries(httplib_interface INTERFACE ws2_32)
    endif()
    
    # Link httplib to REST_Client
    target_link_libraries(REST_Client PRIVATE httplib_interface)
    target_compile_definitions(REST_Client PRIVATE USE_HTTPLIB)
    
    message(STATUS "cpp-httplib source dir: ${httplib_SOURCE_DIR}")
endif()

# === CPR Implementation ===  
if(NOT USE_HTTPLIB)
	set(USE_CPR ON)
    message(STATUS "Using CPR REST client implementation")
	target_compile_definitions(REST_Client PRIVATE USE_CPR)

	message(STATUS "Setting USE_CPR definition (no value)")
    get_target_property(CPR_DEFS REST_Client COMPILE_DEFINITIONS)
    message(STATUS "REST_Client compile definitions: ${CPR_DEFS}")

    # Add implementation source
    target_sources(REST_Client PRIVATE PluginRESTService_Cpr.cpp)
    
    # Fetch CPR dependency
    include(FetchContent)
    FetchContent_Declare(cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1.10.5
    )
    
    # Configure CPR options
    set(CPR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(CPR_BUILD_TESTS_SSL OFF CACHE BOOL "" FORCE)
    set(CPR_ENABLE_SSL ON CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(cpr)
    
    # Link CPR to REST_Client
    target_link_libraries(REST_Client PRIVATE cpr::cpr)
    target_compile_definitions(REST_Client PRIVATE USE_CPR)
endif()

# === Common Configuration ===

# Include directories
target_include_directories(REST_Client 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link common dependencies
target_link_libraries(REST_Client 
    PUBLIC
        logger  # For logging support
)

# Set output directories
set_target_properties(REST_Client PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Platform-specific configuration
if(WIN32)
    set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client" PREFIX "")
    target_compile_definitions(REST_Client PRIVATE PLATFORM_WINDOWS)
else()
    set_target_properties(REST_Client PROPERTIES OUTPUT_NAME "REST_Client")
    target_compile_definitions(REST_Client PRIVATE PLATFORM_LINUX)
endif()

# Debug information
message(STATUS "REST_Client configuration:")
message(STATUS "  - Implementation: ${USE_HTTPLIB} (HttpLib) / ${USE_CPR} (CPR)")
message(STATUS "  - SSL Support: ${HTTPLIB_ENABLE_SSL}")
message(STATUS "  - Output directory: ${CMAKE_BINARY_DIR}/lib")

# Export target for use by other parts of the project
set_target_properties(REST_Client PROPERTIES EXPORT_NAME RESTClient)