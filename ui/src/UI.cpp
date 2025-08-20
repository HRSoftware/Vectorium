
#include "UI.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "EngineUIBridge.h"
#include "ImGuiContextManager.h"
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
	// Init GLFW first
	if (!glfwInit())
	{
		m_logger->log(LogLevel::Error, "Failed to initialize GLFW");
		return false;
	}

	m_logger->log(LogLevel::Info, "GLFW initialized successfully");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(1280, 720, appName.c_str(), nullptr, nullptr);

	if (!window)
	{
		m_logger->log(LogLevel::Error, "Failed to create window");
		glfwTerminate();
		return false;
	}

	m_logger->log(LogLevel::Info, "GLFW window created successfully");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Vsync

	// Validate GLFW setup before ImGui
	m_logger->log(LogLevel::Info, std::format("GLFW window handle: {}", static_cast<void*>(window)));

	// Check GLFW version
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	m_logger->log(LogLevel::Info, std::format("GLFW version: {}.{}.{}", major, minor, revision));

	// Verify OpenGL context
	const char* gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	if (gl_version) 
	{
		m_logger->log(LogLevel::Info, std::format("OpenGL version: {}", gl_version));
	} else {
		m_logger->log(LogLevel::Error, "Failed to get OpenGL version - context might not be active");
		return false;
	}

	m_logger->log(LogLevel::Info, "Testing pure ImGui initialization...");

	// Completely bypass context manager - pure ImGui
	IMGUI_CHECKVERSION();
	m_logger->log(LogLevel::Info, "Initializing ImGui through context manager...");

	// Use the context manager
	auto& contextMgr = ImGuiContextManager::getInstance();
	if (!contextMgr.initialise()) {
		m_logger->log(LogLevel::Error, "Failed to initialize ImGui context manager");
		return false;
	}

	// Ensure context is active
	if (!contextMgr.ensureContextActive()) {
		m_logger->log(LogLevel::Error, "Failed to activate ImGui context");
		return false;
	}

	m_logger->log(LogLevel::Info, "ImGui context manager ready, initializing backends...");

	// Initialize backends
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
		m_logger->log(LogLevel::Error, "Failed to init ImGui GLFW backend");
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init("#version 330")) {
		m_logger->log(LogLevel::Error, "Failed to init ImGui OpenGL backend");
		return false;
	}

	// Set style and build fonts
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Build();

	m_logger->log(LogLevel::Info, "ImGui initialization complete");

	// Create UI bridge
	uiBridge = std::make_unique<EngineUIBridge>(*engine.getPluginManager(),
		*engine.getDataPacketRegistry(),
		*engine.getLogger(),
		*engine.getLogSink(),
		engine.getEngineSettings());

	m_logger->log(LogLevel::Info, "UI initialization complete");
	return true;
}

void UI::render() const
{
	if (!window)
	{
		m_logger->log(LogLevel::Error, "No window to render to");
		return;
	}

	m_logger->log(LogLevel::Debug, "UI::render() - Starting ImGui frame");

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Verify we're in a valid ImGui frame
	ImGuiIO& io = ImGui::GetIO();
	if (!io.Fonts->IsBuilt())
	{
		m_logger->log(LogLevel::Error, "ImGui fonts not available during render");
		ImGui::EndFrame(); // Clean up the frame
		return;
	}

	m_logger->log(LogLevel::Debug, "UI::render() - About to call uiBridge->draw()");
	// GUI rendering - this calls plugin onRender() methods
	if (uiBridge)
	{
		uiBridge->draw();
	}

	m_logger->log(LogLevel::Debug, "UI::render() - Finished uiBridge->draw(), rendering ImGui");

	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (ImDrawData* draw_data = ImGui::GetDrawData())
	{
		ImGui_ImplOpenGL3_RenderDrawData(draw_data);
	}

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
	auto& contextMgr = ImGuiContextManager::getInstance();

	if(uiBridge)
	{
		uiBridge.reset();
	}
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
