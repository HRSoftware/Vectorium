#pragma once

#include "IPluginUIService.h"
#include <unordered_map>
#include <mutex>
#include <memory>

class ILogger;
//class UIContextProvider;
struct ImGuiContext;

/// <summary>
/// Implementation of IPluginUIService that manages ImGui context sharing with plugins
/// Uses UIContextProvider for safe context management
/// </summary>
class PluginUIService_ImGui : public IPluginUIService
{
public:
	explicit PluginUIService_ImGui(std::shared_ptr<ILogger> logger = nullptr);
	~PluginUIService_ImGui() override;

	// IPluginUIService implementation
	ImGuiContext* getImGuiContext() override;
	void setImGuiContext(ImGuiContext* context) override;

	void registerPluginUIRenderer(const std::string&    pluginName,
	                      std::function<void()> renderCallback) override;
	void unregisterPluginUIRenderer(const std::string& pluginName) override;
	void renderPluginUIs() override;

	bool isContextValid() const override;

	std::string getDiagnosticInfo() const override;
	size_t getRegisteredPluginCount() const override;
	void setErrorCallback(std::function<void(const std::string&)> callback) override;

	private:
		void ensureInitialised();

public:
	bool isUIAvailable() const override;

private:
	std::shared_ptr<ILogger> m_logger;
	ImGuiContext* m_context;

	std::unordered_map<std::string, std::function<void()>> m_pluginCallbacks;
	mutable std::mutex m_callbackMutex;

	std::function<void(const std::string&)> m_errorCallback;
	bool                                     m_initialised;
};