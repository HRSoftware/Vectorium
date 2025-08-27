#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>

struct ImGuiContext;
class ILogger;

/// <summary>
/// Manages ImGui contexts and provides safe context sharing for plugins
/// Centralizes all ImGui context management within the UI module
/// </summary>
class ImGuiContextManager
{
public:
	explicit ImGuiContextManager(std::shared_ptr<ILogger> logger = nullptr);
	~ImGuiContextManager();

	// Core context management
	bool initialise();
	void shutdown();

	/// <summary>
	/// Gets the main application's ImGui context
	/// </summary>
	ImGuiContext* getContext() const;

	/// <summary>
	/// Sets the main application context (called during UI initialization)
	/// </summary>
	void setContext(ImGuiContext* context);

	/// <summary>
	/// Executes ImGui code with the main context safely set
	/// Automatically handles context switching and error recovery
	/// </summary>
	bool executeWithMainContext(const std::function<void()>& imguiCode, 
		const std::string& contextName = "Unknown") const;

	/// <summary>
	/// Registers a plugin's UI render function
	/// </summary>
	void registerPluginRender(const std::string& pluginName, 
		std::function<void()> renderFunction);

	/// <summary>
	/// Unregisters a plugin's UI render function
	/// </summary>
	void unregisterPluginRender(const std::string& pluginName);

	/// <summary>
	/// Renders all registered plugin UIs
	/// Called by the main UI loop
	/// </summary>
	void renderAllPluginUIs();

	/// <summary>
	/// Checks if the main context is valid and ready for use
	/// </summary>
	bool isContextValid() const;

	/// <summary>
	/// Gets context information for debugging
	/// </summary>
	std::string getContextInfo() const;
	void        setErrorCallback(std::function<void(const std::string&)> callback);

	// Statistics and debugging
	size_t getRegisteredPluginCount() const;
	std::vector<std::string> getRegisteredPluginNames() const;

private:
	std::shared_ptr<ILogger> m_logger;
	ImGuiContext* m_mainContext;
	bool m_initialised;

	// Plugin render functions
	std::unordered_map<std::string, std::function<void()>> m_pluginRenderFunctions;
	mutable std::mutex m_pluginMutex;

	// Error handling
	std::function<void(const std::string&)> m_errorCallback;

	// Internal helpers
	void logError(const std::string& message) const;
	void logInfo(const std::string& message) const;
	void logDebug(const std::string& message) const;
	bool safeContextSwitch(ImGuiContext* newContext, const std::string& operation) const;
};