#include "UI/ImGuiContextManager.h"
#include <algorithm>
#include <format>
#include <imgui.h>
#include <ranges>
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"

ImGuiContextManager::ImGuiContextManager(std::shared_ptr<ILogger> logger)
	: m_logger(std::move(logger))
	, m_mainContext(nullptr)
	, m_initialised(false)
{
	logInfo("ImGuiContextManager created");
}

ImGuiContextManager::~ImGuiContextManager()
{
	shutdown();
	logInfo("ImGuiContextManager destroyed");
}

bool ImGuiContextManager::initialise()
{
	if (m_initialised)
	{
		logDebug("ImGuiContextManager already initialized");
		return true;
	}

	// Get the current ImGui context (should be set by main UI)
	m_mainContext = ImGui::GetCurrentContext();

	if (!m_mainContext)
	{
		logError("No ImGui context available during initialization");
		return false;
	}

	m_initialised = true;
	logInfo(std::format("ImGuiContextManager initialized with context: {}", static_cast<void*>(m_mainContext)));
	return true;
}

void ImGuiContextManager::shutdown()
{
	if (!m_initialised)
	{
		return;
	}

	logInfo("Shutting down ImGuiContextManager");

	// Clear all plugin render functions
	{
		std::lock_guard<std::mutex> lock(m_pluginMutex);
		m_pluginRenderFunctions.clear();
	}

	m_mainContext = nullptr;
	m_initialised = false;

	logInfo("ImGuiContextManager shutdown complete");
}

ImGuiContext* ImGuiContextManager::getContext() const
{
	return m_mainContext;
}

void ImGuiContextManager::setContext(ImGuiContext* context)
{
	m_mainContext = context;

	if (context)
	{
		logInfo(std::format("Main context set to: {}", static_cast<void*>(context)));
	}
	else
	{
		logInfo("Main context set to nullptr");
	}
}

bool ImGuiContextManager::executeWithMainContext(const std::function<void()>& imguiCode, const std::string& contextName) const
{
	if (!isContextValid() || !imguiCode)
	{
		logError(std::format("Cannot execute ImGui code for '{}': invalid context or code", contextName));
		return false;
	}

	// Store current context to restore later
	ImGuiContext* previousContext = ImGui::GetCurrentContext();

	try
	{
		// Switch to main context if needed
		if (previousContext != m_mainContext)
		{
			if (!safeContextSwitch(m_mainContext, std::format("executing {}", contextName)))
			{
				return false;
			}
		}

		// Execute the ImGui code
		imguiCode();

		// Restore previous context if it was different
		if (previousContext != m_mainContext)
		{
			safeContextSwitch(previousContext, std::format("restoring after {}", contextName));
		}

		return true;

	}
	catch (const std::exception& e)
	{
		logError(std::format("Exception during ImGui execution for '{}': {}", contextName, e.what()));

		// Try to restore context
		try
		{
			if (previousContext != m_mainContext)
			{
				safeContextSwitch(previousContext, "error recovery");
			}
		}
		catch (...)
		{
			logError("Failed to restore context during error recovery");
			if (m_errorCallback) {
				m_errorCallback("Critical: Failed to restore ImGui context");
			}
		}

		return false;
	}
	catch (...)
	{
		logError(std::format("Unknown exception during ImGui execution for '{}'", contextName));

		// Try to restore context
		try
		{
			if (previousContext != m_mainContext)
			{
				safeContextSwitch(previousContext, "error recovery");
			}
		}
		catch (...)
		{
			logError("Failed to restore context during error recovery");
			if (m_errorCallback) {
				m_errorCallback("Critical: Failed to restore ImGui context");
			}
		}

		return false;
	}
}

void ImGuiContextManager::registerPluginRender(const std::string& pluginName, std::function<void()> renderFunction)
{
	std::lock_guard<std::mutex> lock(m_pluginMutex);

	m_pluginRenderFunctions[pluginName] = std::move(renderFunction);

	logInfo(std::format("Registered UI render function for plugin: {}", pluginName));
}

void ImGuiContextManager::unregisterPluginRender(const std::string& pluginName)
{
	std::lock_guard<std::mutex> lock(m_pluginMutex);

	auto it = m_pluginRenderFunctions.find(pluginName);
	if (it != m_pluginRenderFunctions.end())
	{
		m_pluginRenderFunctions.erase(it);
		logInfo(std::format("Unregistered UI render function for plugin: {}", pluginName));
	}
	else
	{
		logDebug(std::format("Attempted to unregister non-existent plugin: {}", pluginName));
	}
}

void ImGuiContextManager::renderAllPluginUIs()
{
	if (!isContextValid())
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_pluginMutex);

	for (const auto& [pluginName, renderFunction] : m_pluginRenderFunctions)
	{
		if (renderFunction)
		{
			bool success = executeWithMainContext(renderFunction, std::format("plugin UI: {}", pluginName));

			if (!success)
			{
				logError(std::format("Failed to render UI for plugin: {}", pluginName));
			}
		}
	}
}

bool ImGuiContextManager::isContextValid() const
{
	return m_initialised && m_mainContext != nullptr;
}

std::string ImGuiContextManager::getContextInfo() const
{
	if (!isContextValid())
	{
		return "Context: Invalid";
	}

	ImGuiContext* currentContext = ImGui::GetCurrentContext();

	return std::format("Main Context: {}, Current Context: {}, Valid: {}, Plugins: {}",
		static_cast<void*>(m_mainContext),
		static_cast<void*>(currentContext),
		(currentContext != nullptr),
		getRegisteredPluginCount());
}

void ImGuiContextManager::setErrorCallback(std::function<void(const std::string&)> callback)
{
	m_errorCallback = std::move(callback);
}

size_t ImGuiContextManager::getRegisteredPluginCount() const
{
	std::lock_guard<std::mutex> lock(m_pluginMutex);
	return m_pluginRenderFunctions.size();
}

std::vector<std::string> ImGuiContextManager::getRegisteredPluginNames() const
{
	std::lock_guard<std::mutex> lock(m_pluginMutex);

	std::vector<std::string> names;
	names.reserve(m_pluginRenderFunctions.size());

	for (const auto& name : m_pluginRenderFunctions | std::views::keys)
	{
		names.push_back(name);
	}

	std::ranges::sort(names);
	return names;
}

// Private helper methods
void ImGuiContextManager::logError(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Error, std::format("[ImGuiContextManager] {}", message));
	}
}

void ImGuiContextManager::logInfo(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Info, std::format("[ImGuiContextManager] {}", message));
	}
}

void ImGuiContextManager::logDebug(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Debug, std::format("[ImGuiContextManager] {}", message));
	}
}

bool ImGuiContextManager::safeContextSwitch(ImGuiContext* newContext, 
	const std::string& operation) const
{
	try
	{
		ImGui::SetCurrentContext(newContext);
		logDebug(std::format("Context switched to {} for operation: {}", static_cast<void*>(newContext), operation));
		return true;
	}
	catch (const std::exception& e)
	{
		logError(std::format("Failed to switch context for {}: {}", operation, e.what()));
		return false;
	}
	catch (...)
	{
		logError(std::format("Unknown error switching context for {}", operation));
		return false;
	}
}