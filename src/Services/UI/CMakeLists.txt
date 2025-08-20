# Create UI services library
add_library(ui_services STATIC
    "ImGuiContextManager.cpp"
)

target_include_directories(ui_services PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
)

target_link_libraries(ui_services PUBLIC 
imgui_static)

# Add export definitions to ui_services

target_compile_definitions(ui_services PRIVATE VECTORIUM_EXPORTS)
set_target_properties(ui_services PROPERTIES CXX_STANDARD 23)