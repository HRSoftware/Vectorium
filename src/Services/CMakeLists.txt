cmake_minimum_required(VERSION 3.16)

add_library(services_core INTERFACE)

target_include_directories(services_core INTERFACE 
	${PROJECT_SOURCE_DIR}/include)

target_compile_features(services_core INTERFACE cxx_std_23)

add_subdirectory(Logging)
#add_subdirectory(Mocking)
add_subdirectory(REST)

add_library(services INTERFACE)

target_link_libraries(services INTERFACE
	services_core
	services_logging
	services_REST)

# Export targets for use by other parts of the project
add_library(Vectorium::services_core ALIAS services_core)
add_library(Vectorium::services ALIAS services)