#include "Services/UI/UIService_ImGui.h"
#include "Services/UI/IUIContextProvider.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include "Services/UI/UIContextProvider.h"
#include <imgui.h>
#include <format>
#include <ranges>

UIService_ImGui::UIService_ImGui(std::shared_ptr<ILogger> logger)
	: m_logger(std::move(logger))
	, m_context(nullptr)
{
	std::cout << "UIService_ImGui constructor starting..." << std::endl;

	// Only do basic initialization, NO ImGui calls
	m_initialised = false;

	std::cout << "UIService_ImGui constructor completed successfully" << std::endl;
}

UIService_ImGui::~UIService_ImGui()
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "[UIService_ImGui] Shutting down");
	}

	// Clear all plugin callbacks
	{
		std::lock_guard<std::mutex> lock(m_callbackMutex);
		m_pluginCallbacks.clear();
	}
}

ImGuiContext* UIService_ImGui::getImGuiContext()
{
	return m_context;
}

void UIService_ImGui::setImGuiContext(ImGuiContext* context)
{
	m_context = context;

	// Update context provider
	if (m_contextProvider)
	{
		m_contextProvider->setContext(context);
	}

	if (m_logger)
	{
		if (context)
		{
			m_logger->log(LogLevel::Info, 
				std::format("[UIService_ImGui] Context set to: {}", static_cast<void*>(context)));
		} else {
			m_logger->log(LogLevel::Warning, "[UIService_ImGui] Context set to nullptr");
		}
	}
}

void UIService_ImGui::registerPluginUI(const std::string& pluginName, std::function<void()> renderCallback)
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);
	ensureInitialised();  // Initialize when first used
	m_pluginCallbacks[pluginName] = std::move(renderCallback);

	if (m_logger) {
		m_logger->log(LogLevel::Info, 
			std::format("[UIService_ImGui] Registered UI for plugin: {}", pluginName));
	}
}

void UIService_ImGui::unregisterPluginUI(const std::string& pluginName)
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);

	auto it = m_pluginCallbacks.find(pluginName);
	if (it != m_pluginCallbacks.end()) {
		m_pluginCallbacks.erase(it);

		if (m_logger) {
			m_logger->log(LogLevel::Info, 
				std::format("[UIService_ImGui] Unregistered UI for plugin: {}", pluginName));
		}
	}
}

void UIService_ImGui::renderPluginUIs()
{
	if (!isContextValid()) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_callbackMutex);

	for (const auto& [pluginName, callback] : m_pluginCallbacks) {
		if (callback) {
			bool success = m_contextProvider->executeWithContext(
				callback, 
				std::format("Plugin UI: {}", pluginName)
			);

			if (!success && m_logger) {
				m_logger->log(LogLevel::Error, 
					std::format("[UIService_ImGui] Failed to render UI for plugin: {}", pluginName));
			}
		}
	}
}

bool UIService_ImGui::isContextValid() const
{
	return m_context != nullptr && m_contextProvider && m_contextProvider->hasValidContext();
}

bool UIService_ImGui::executeWithContext(std::function<void()> imguiCode, 
	const std::string& operationName)
{
	if (!m_contextProvider) {
		if (m_logger) {
			m_logger->log(LogLevel::Error, 
				std::format("[UIService_ImGui] No context provider for operation: {}", operationName));
		}
		return false;
	}

	return m_contextProvider->executeWithContext(imguiCode, operationName);
}

std::string UIService_ImGui::getDiagnosticInfo() const
{
	std::string info;

	info += std::format("UIService_ImGui Diagnostics:\n");
	info += std::format("\tContext Valid: {}\n", isContextValid());
	info += std::format("\tStored Context: {}\n", static_cast<void*>(m_context));
	info += std::format("\tCurrent Context: {}\n", static_cast<void*>(ImGui::GetCurrentContext()));
	info += std::format("\tRegistered Plugins: {}\n", getRegisteredPluginCount());

	if (m_contextProvider)
	{
		info += std::format("  Provider Info: {}\n", m_contextProvider->getContextInfo());
	}
	else
	{
		info += "  Provider: Not available\n";
	}

	// List registered plugins
	{
		std::lock_guard<std::mutex> lock(m_callbackMutex);
		if (!m_pluginCallbacks.empty())
		{
			info += "  Plugin List:\n";
			for (const auto& name : m_pluginCallbacks | std::views::keys)
			{
				info += std::format("\t\t- {}\n", name);
			}
		}
	}

	return info;
}

size_t UIService_ImGui::getRegisteredPluginCount() const
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);
	return m_pluginCallbacks.size();
}

void UIService_ImGui::setErrorCallback(std::function<void(const std::string&)> callback)
{
	m_errorCallback = callback;

	// Also set on context provider
	if (m_contextProvider) {
		m_contextProvider->setErrorCallback(callback);
	}

	if (m_logger) {
		m_logger->log(LogLevel::Debug, "[UIService_ImGui] Error callback set");
	}
}

void UIService_ImGui::ensureInitialised()
{
	if (!m_initialised && ImGui::GetCurrentContext()) 
	{
		// Do actual ImGui initialization here, not in constructor
		std::cout << "Initializing UIService_ImGui on first use" << std::endl;
		m_initialised = true;
	}
}
