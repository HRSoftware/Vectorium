#pragma once

#include <memory>
#include <string>

class ImGuiContextManager;
class IUIService;
class ILogger;
struct ImGuiContext;

/// <summary>
/// Central coordinator for all UI services
/// Manages the relationship between context management and service provision
/// </summary>
class UIServiceManager
{
public:
	explicit UIServiceManager(std::shared_ptr<ILogger> logger = nullptr);
	~UIServiceManager();

	// Core lifecycle
	bool initialise();
	void shutdown();

	/// <summary>
	/// Updates the main ImGui context (called when UI is initialized/changed)
	/// </summary>
	/// <param name="context">The new main ImGui context</param>
	void updateMainContext(ImGuiContext* context) const;

	/// <summary>
	/// Renders all registered plugin UIs
	/// Called by the main UI render loop
	/// </summary>
	void renderPluginUIs() const;

	/// <summary>
	/// Gets the UI service for plugins to use
	/// </summary>
	/// <returns>Shared pointer to the UI service interface</returns>
	std::shared_ptr<IUIService> getUIService();

	/// <summary>
	/// Gets the context manager for advanced use cases
	/// </summary>
	/// <returns>Pointer to the context manager (may be null)</returns>
	ImGuiContextManager* getContextManager() const;

	/// <summary>
	/// Checks if the service manager is properly initialised
	/// </summary>
	/// <returns>True if initialized and ready</returns>
	bool isInitialised() const;

	/// <summary>
	/// Gets status information for debugging
	/// </summary>
	/// <returns>Multi-line string with status information</returns>
	std::string getStatusInfo() const;

private:
	std::shared_ptr<ILogger> m_logger;
	bool m_initialised;

	// Core components
	std::unique_ptr<ImGuiContextManager> m_contextManager;
	std::shared_ptr<IUIService> m_imguiService;

	// Logging helpers
	void logError(const std::string& message) const;
	void logInfo(const std::string& message) const;
	void logDebug(const std::string& message) const;
};