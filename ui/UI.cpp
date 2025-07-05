
#include "UI.h"
#include "EngineUIBridge.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include "../include/Engine.h"

const std::string appName = "Vectorium";

UI::UI(Engine& engine) : engine(engine)
{
}

bool UI::init()
{
	// Init GLFW
	if (!glfwInit())
	{
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(1280, 720, appName.c_str(), nullptr, nullptr);

	if (!window)
	{
		std::cout << "[UI::init] - failed to create window\n";
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Vsync

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();


	uiBridge = std::make_unique<EngineUIBridge>(engine.getPluginManager(), engine.getDataPacketRegistry());

	std::cout << "[UI::init] - complete\n";
	return true;
}

void UI::render() const
{
	if (!window)
	{
		std::cout << "[UI::Render] - no window to render to\n";
		return;
	}

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// GUI
	ImGui::Begin("Plugin Dashboard");
	ImGui::Text("Welcome to the Modular Data Engine");
	// TODO: Display loaded plugins, packet types, debug info
	ImGui::End();

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

bool UI::shouldClose() const
{
	if(window)
	{
		return glfwWindowShouldClose(window);
	}

	return false;
}

void UI::shutdown()
{
	if(uiBridge)
	// Cleanup
	if(window)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(window);

		window = nullptr;
	}

	glfwTerminate();
	std::cout << "[UI::shutdown] - complete\n";
}
