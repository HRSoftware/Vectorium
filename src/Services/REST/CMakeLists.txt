cmake_minimum_required(VERSION 3.16)

add_library(services_REST STATIC)

set_target_properties(services_REST PROPERTIES 
    CXX_STANDARD 23
    POSITION_INDEPENDENT_CODE ON
)

set(USE_HTTPLIB OFF CACHE BOOL "Use cpp-httplib as REST client backend. Fall back to CPR otherwise" FORCE)

target_sources(services_REST PRIVATE 
	"PluginRESTServiceFactory.cpp")

# === cpp-httplib Implementation ===
if(USE_HTTPLIB)
    message(STATUS "Using cpp-httplib REST client implementation")
    
	target_compile_definitions(services_REST PRIVATE USE_HTTPLIB)

    # Add implementation source
    target_sources(services_REST PRIVATE "PluginRESTService_HttpLib.cpp")
    
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
    
    # Link httplib to services_REST
    target_link_libraries(services_REST PRIVATE httplib_interface)
    target_compile_definitions(services_REST PRIVATE USE_HTTPLIB)
    
    message(STATUS "cpp-httplib source dir: ${httplib_SOURCE_DIR}")
endif()

# === CPR Implementation ===  
if(NOT USE_HTTPLIB)
	target_compile_definitions(services_REST PRIVATE USE_CPR)

    get_target_property(CPR_DEFS services_REST COMPILE_DEFINITIONS)

    # Add implementation source
    target_sources(services_REST PRIVATE "PluginRESTService_Cpr.cpp")
    
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
    
    # Link CPR to services_REST
    target_link_libraries(services_REST PRIVATE cpr::cpr)
    target_compile_definitions(services_REST PRIVATE USE_CPR)
endif()

# === Common Configuration ===

# Include directories
target_include_directories(services_REST 
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link common dependencies
target_link_libraries(services_REST PUBLIC
		services_core
        services_logging  # For logging support
)

# Set output directories
set_target_properties(services_REST PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Platform-specific configuration
if(WIN32)
    set_target_properties(services_REST PROPERTIES OUTPUT_NAME "services_REST" PREFIX "")
    target_compile_definitions(services_REST PRIVATE PLATFORM_WINDOWS)
else()
    set_target_properties(services_REST PROPERTIES OUTPUT_NAME "services_REST")
    target_compile_definitions(services_REST PRIVATE PLATFORM_LINUX)
endif()

# Debug information
message(STATUS "services_REST configuration:")
if(${USE_HTTPLIB})
	message(STATUS "  - Implementation: HttpLib")
	message(STATUS "  - SSL Support: ${HTTPLIB_ENABLE_SSL}")
else()
	message(STATUS "  - Implementation: CPR")
endif()

message(STATUS "  - Output directory: ${CMAKE_BINARY_DIR}/lib")

# Export target for use by other parts of the project
#set_target_properties(services_REST PROPERTIES EXPORT_NAME RESTClient)