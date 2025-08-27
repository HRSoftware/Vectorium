#include "UI/UI.h"
#include "EngineUIBridge.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "Services/UI/PluginUIService_ImGui.h"

const std::string appName = "Vectorium";

UI::UI(IEngineUIBridge& engineBridge)
	: m_engineBridge(engineBridge)
	, m_window(nullptr)
	, m_initialised(false)
{
	// Get logger through interface - create non-owning shared_ptr
	m_logger = std::shared_ptr<ILogger>(m_engineBridge.getLogger());
	if (m_logger)
	{
		m_logger->setPluginName("UI");
		m_logger->log(LogLevel::Info, "UI created with engine bridge interface");
	}
}

UI::~UI() = default;

bool UI::init()
{
	if (m_initialised)
	{
		if (m_logger)
		{
			m_logger->log(LogLevel::Warning, "UI already initialized");
		}
		return true;
	}

	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "Initializing UI system...");
	}

	// Initialise in sequence
	if (!initialiseGLFW())
	{
		return false;
	}

	if (!createWindow())
	{
		cleanupGLFW();
		return false;
	}

	if (!initialiseImGui())
	{
		cleanupGLFW();
		return false;
	}

	setupImGuiStyle();

	createUIService();

	// Create UI bridge for main application UI
	m_uiBridge = std::make_unique<EngineUIBridge>(m_engineBridge);// - duplicate?

// IMPORTANT: Notify engine that UI is ready and ImGui context is created
	m_engineBridge.notifyUIInitialised();
	m_initialised = true;

	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "UI initialization complete");
	}

	return true;
}

void UI::render() const
{
	if (!m_initialised || !m_window)
	{
		if (m_logger) {
			m_logger->log(LogLevel::Error, "Cannot render - UI not initialized");
		}
		return;
	}

	handleWindowEvents();

	// Start ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Render main application UI
	renderMainUI();

	// Render plugin UIs (handled by EngineUIBridge)
	renderPluginUIs();

	// Finish ImGui rendering
	ImGui::Render();

	// OpenGL rendering
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

	// Check if UI requested application quit
	if (m_uiBridge && m_uiBridge->shouldQuit())
	{
		glfwSetWindowShouldClose(m_window, GLFW_TRUE);
	}
}

bool UI::shouldClose() const
{
	return m_window ? glfwWindowShouldClose(m_window) : true;
}

void UI::shutdown()
{
	if (!m_initialised)
	{
		return;
	}

	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "Shutting down UI system...");
	}

	if (m_uiService)
	{
		m_engineBridge.setUIService(nullptr);  // Remove from engine
		m_uiService.reset();  // UI releases ownership
	}

	// Cleanup in reverse order
	m_uiBridge.reset();
	cleanupImGui();
	cleanupGLFW();

	m_initialised = false;

	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "UI shutdown complete");
	}

	m_logger.reset();
}

std::shared_ptr<IPluginUIService> UI::getUIService() const
{
	return m_uiService;
}

void UI::createUIService()
{
	if (ImGui::GetCurrentContext())
	{
		ImGuiIO& io = ImGui::GetIO();

		m_logger->log(LogLevel::Debug, std::format(
			"ImGui initialized: {}\n"
			"Display size: {}x{}\n"
			"Backend flags: {}\n",
			io.BackendFlags != 0,
			io.DisplaySize.x, io.DisplaySize.y,
			io.BackendFlags
		));
	}

	try
	{
		m_logger->log(LogLevel::Info, "Creating UI service...");

		//Create the UI and set the ImGui context for it
		m_uiService = std::make_shared<PluginUIService_ImGui>(); // How to add logger to this service?
		m_uiService->setImGuiContext(ImGui::GetCurrentContext());
		m_engineBridge.setUIService(m_uiService);

		m_logger->log(LogLevel::Info, "UI service created and provided to engine");

	}
	catch (const std::exception& e)
	{
		m_logger->log(LogLevel::Error, std::format("Exception during PluginUIService_ImGui creation: {}\n", e.what()));
		m_uiService = nullptr;
	}

	m_logger->log(LogLevel::Debug, "=== End createUIService() Debug ===\n");
}

// Private helper methods
bool UI::initialiseGLFW() const
{
	if (!glfwInit())
	{
		if (m_logger)
		{
			m_logger->log(LogLevel::Error, "Failed to initialize GLFW");
		}
		return false;
	}
	return true;
}

bool UI::createWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(1280, 720, appName.c_str(), nullptr, nullptr);
	if (!m_window)
	{
		if (m_logger)
		{
			m_logger->log(LogLevel::Error, "Failed to create GLFW window");
		}
		return false;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync
	return true;
}

bool UI::initialiseImGui() const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Debug, "Setting up ImGui...");
	}

	// Check these are all called
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Make sure these complete successfully
	if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true))
	{
		if (m_logger)
		{
			m_logger->log(LogLevel::Error, "Failed to initialize ImGui GLFW implementation");
		}
		return false;
	}

	if (!ImGui_ImplOpenGL3_Init("#version 330")) {
		if (m_logger)
		{
			m_logger->log(LogLevel::Error, "Failed to initialize ImGui OpenGL3 implementation");
		}
		return false;
	}

	// Test ImGui is working
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (m_logger)
	{
		m_logger->log(LogLevel::Debug, std::format("ImGui initialized - Display: {}x{}", 
			io.DisplaySize.x, io.DisplaySize.y));
	}

	return true;
}

void UI::setupImGuiStyle()
{
	ImGui::StyleColorsDark();
	// Additional style customization could go here
}

void UI::handleWindowEvents() const
{
	glfwPollEvents();
}

void UI::renderMainUI() const
{
	if (m_uiBridge)
	{
		m_uiBridge->draw();
	}
}

void UI::renderPluginUIs() const
{
	// Plugin UIs are rendered as part of EngineUIBridge::draw()
	// which calls EngineUIBridge::drawPluginUI()
	// This happens automatically in renderMainUI() above
	//m_engineBridge.getPluginManager()->renderAllPlugins();

	if(m_uiService && m_uiService->isUIAvailable())
	{
		//const auto imguiService = std::static_pointer_cast<PluginUIService_ImGui>(m_uiService);
		m_uiService->renderPluginUIs();
	}
}

void UI::cleanupImGui()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}

void UI::cleanupGLFW()
{
	if (m_window)
	{
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}
	glfwTerminate();
}