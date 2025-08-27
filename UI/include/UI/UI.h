#pragma once

#include "IEngineUIBridge.h"  // Include interface from Engine module
#include <memory>

class EngineUIBridge;
// Forward declarations - avoid including heavy headers
struct GLFWwindow;
class ILogger;

/// <summary>
/// Main UI class that manages GLFW window, ImGui context, and rendering
/// Uses IEngineUIBridge interface to communicate with Engine without circular dependency
/// </summary>
class UI
{
public:
	/// <summary>
	/// Constructor takes engine interface, not concrete Engine class
	/// This breaks the circular dependency
	/// </summary>
	/// <param name="engineBridge">Interface to engine functionality needed by UI</param>
	explicit UI(IEngineUIBridge& engineBridge);

	/// <summary>
	/// Destructor - cleanup handled automatically by RAII
	/// </summary>
	~UI();

	// Non-copyable, non-movable (manages GLFW resources)
	UI(const UI&) = delete;
	UI& operator=(const UI&) = delete;
	UI(UI&&) = delete;
	UI& operator=(UI&&) = delete;

	/// <summary>
	/// Initializes GLFW, creates window, sets up ImGui context
	/// Notifies engine when UI is ready
	/// </summary>
	/// <returns>True if initialization successful</returns>
	bool init();

	/// <summary>
	/// Main render loop - renders both main UI and plugin UIs
	/// Called every frame from main application loop
	/// </summary>
	void render() const;

	/// <summary>
	/// Checks if user requested window close
	/// </summary>
	/// <returns>True if window should close</returns>
	bool shouldClose() const;

	/// <summary>
	/// Cleanup GLFW, ImGui, and window resources
	/// Safe to call multiple times
	/// </summary>
	void shutdown();

	/// <summary>
	/// Gets the native GLFW window handle
	/// For advanced use cases that need direct window access
	/// </summary>
	/// <returns>GLFW window pointer, or nullptr if not initialized</returns>
	GLFWwindow* getWindow() const { return m_window; }

	/// <summary>
	/// Checks if UI is properly initialized
	/// </summary>
	/// <returns>True if init() was successful and window exists</returns>
	bool isInitialized() const { return m_window != nullptr; }

	std::shared_ptr<IPluginUIService> getUIService() const;
	void                        createUIService();

private:
	// Engine interface - provides access to engine functionality
	IEngineUIBridge& m_engineBridge;

	// UI components
	std::unique_ptr<EngineUIBridge> m_uiBridge;  // Handles main application UI
	GLFWwindow* m_window = nullptr;                // GLFW window handle
	std::shared_ptr<ILogger> m_logger;           // Logger for UI messages

	// UI state
	bool m_initialised = false;
	std::shared_ptr<IPluginUIService>  m_uiService;

	// Private helper methods
	bool initialiseGLFW() const;
	bool createWindow();
	bool initialiseImGui() const;
	void setupImGuiStyle();
	void handleWindowEvents() const;
	void renderMainUI() const;
	void renderPluginUIs() const;
	void cleanupImGui();
	void cleanupGLFW();
};