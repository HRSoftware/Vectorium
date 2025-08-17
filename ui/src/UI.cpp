
#include "UI.h"
#include "EngineUIBridge.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"

const std::string appName = "Vectorium";

UI::UI(Engine& engine) : engine(engine)
{
	m_logger = engine.getLogger();
	m_logger->setPluginName("UI");
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
		m_logger->log(LogLevel::Error, "Failed to create window");
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

	//Enable docking
	/*ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;*/


	uiBridge = std::make_unique<EngineUIBridge>(*engine.getPluginManager(),
		*engine.getDataPacketRegistry(),
		*engine.getLogger(),
		*engine.getLogSink(),
		engine.getEngineSettings());

	m_logger->log(LogLevel::Info, "Init complete");
	return true;
}

void UI::render() const
{
	if (!window)
	{
		m_logger->log(LogLevel::Error, "No window to render to");
		return;
	}

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// GUI
	uiBridge->draw();

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);

	if(uiBridge->shouldQuit())
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
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
	m_logger->log(LogLevel::Info, "Shutdown Complete");
}
