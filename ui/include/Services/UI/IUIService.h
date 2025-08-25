#pragma once

#include <functional>
#include <memory>
#include <string>

// Forward declarations
struct ImGuiContext;
class ILogger;

/// <summary>
/// Interface for UI services that can be provided to plugins
/// Allows plugins to render UI elements using the main application's ImGui context
/// This is the main interface that plugins will interact with
/// </summary>
class IUIService
{
public:
	virtual ~IUIService() = default;

	/// <summary>
	/// Gets the current ImGui context that plugins should use
	/// </summary>
	/// <returns>Pointer to the shared ImGui context</returns>
	virtual ImGuiContext* getImGuiContext() = 0;

	/// <summary>
	/// Sets the ImGui context (called by main application)
	/// </summary>
	/// <param name="context">The ImGui context to share with plugins</param>
	virtual void setImGuiContext(ImGuiContext* context) = 0;

	/// <summary>
	/// Registers a plugin UI render callback
	/// This is the primary method plugins use to register their UI
	/// </summary>
	/// <param name="pluginName">Name of the plugin</param>
	/// <param name="renderCallback">Function to call during UI rendering</param>
	virtual void registerPluginUI(const std::string& pluginName, 
		std::function<void()> renderCallback) = 0;

	/// <summary>
	/// Unregisters a plugin's UI callbacks
	/// </summary>
	/// <param name="pluginName">Name of the plugin to unregister</param>
	virtual void unregisterPluginUI(const std::string& pluginName) = 0;

	/// <summary>
	/// Renders all registered plugin UIs (called by main application)
	/// </summary>
	virtual void renderPluginUIs() = 0;

	/// <summary>
	/// Checks if ImGui context is available and valid
	/// </summary>
	/// <returns>True if context is valid</returns>
	virtual bool isContextValid() const = 0;

	/// <summary>
	/// Helper function for plugins to safely execute ImGui code
	/// Automatically sets context before execution and handles errors
	/// </summary>
	/// <param name="imguiCode">Lambda containing ImGui calls</param>
	/// <param name="operationName">Name for debugging/logging</param>
	/// <returns>True if execution was successful</returns>
	virtual bool executeWithContext(std::function<void()> imguiCode, 
		const std::string& operationName = "Plugin UI") = 0;

	/// <summary>
	/// Gets diagnostic information about the UI service state
	/// Useful for debugging plugin UI issues
	/// </summary>
	/// <returns>String containing diagnostic information</returns>
	virtual std::string getDiagnosticInfo() const = 0;

	/// <summary>
	/// Gets the number of currently registered plugins
	/// </summary>
	/// <returns>Number of plugins with registered UI callbacks</returns>
	virtual size_t getRegisteredPluginCount() const = 0;

	/// <summary>
	/// Sets an error callback for UI-related errors
	/// Allows the main application to be notified of plugin UI issues
	/// </summary>
	/// <param name="callback">Function to call when UI errors occur</param>
	virtual void setErrorCallback(std::function<void(const std::string&)> callback) = 0;
};