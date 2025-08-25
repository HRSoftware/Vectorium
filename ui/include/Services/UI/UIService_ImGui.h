#pragma once

#include "IUIService.h"
#include <unordered_map>
#include <mutex>
#include <memory>

class ILogger;
class UIContextProvider;
struct ImGuiContext;

/// <summary>
/// Implementation of IUIService that manages ImGui context sharing with plugins
/// Uses UIContextProvider for safe context management
/// </summary>
class UIService_ImGui : public IUIService
{
public:
	explicit UIService_ImGui(std::shared_ptr<ILogger> logger = nullptr);
	~UIService_ImGui() override;

	// IUIService implementation
	ImGuiContext* getImGuiContext() override;
	void setImGuiContext(ImGuiContext* context) override;

	void registerPluginUI(const std::string&    pluginName,
	                      std::function<void()> renderCallback) override;
	void unregisterPluginUI(const std::string& pluginName) override;
	void renderPluginUIs() override;

	bool isContextValid() const override;
	bool executeWithContext(std::function<void()> imguiCode, const std::string& operationName = "Plugin UI") override;

	std::string getDiagnosticInfo() const override;
	size_t getRegisteredPluginCount() const override;
	void setErrorCallback(std::function<void(const std::string&)> callback) override;

	private:
		void ensureInitialised();

private:
	std::shared_ptr<ILogger> m_logger;
	ImGuiContext* m_context;
	std::unique_ptr<UIContextProvider> m_contextProvider;

	std::unordered_map<std::string, std::function<void()>> m_pluginCallbacks;
	mutable std::mutex m_callbackMutex;

	std::function<void(const std::string&)> m_errorCallback;
	bool                                     m_initialised;
};