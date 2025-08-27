
#include "Services/UI/PluginUIService_ImGui.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include <imgui.h>
#include <format>
#include <ranges>

PluginUIService_ImGui::PluginUIService_ImGui(std::shared_ptr<ILogger> logger)
	: m_logger(std::move(logger))
	, m_context(nullptr)
{
	std::cout << "PluginUIService_ImGui constructor starting..." << std::endl;

	// Only do basic initialisation, NO ImGui calls
	m_initialised = false;

	std::cout << "PluginUIService_ImGui constructor completed successfully" << std::endl;
}

PluginUIService_ImGui::~PluginUIService_ImGui()
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Info, "[PluginUIService_ImGui] Shutting down");
	}

	// Clear all plugin callbacks
	{
		std::lock_guard<std::mutex> lock(m_callbackMutex);
		m_pluginCallbacks.clear();
	}
}

ImGuiContext* PluginUIService_ImGui::getImGuiContext()
{
	return m_context;
}

void PluginUIService_ImGui::setImGuiContext(ImGuiContext* context)
{
	m_context = context;

	if (m_logger)
	{
		if (context)
		{
			m_logger->log(LogLevel::Info, 
				std::format("[PluginUIService_ImGui] Context set to: {}", static_cast<void*>(context)));
		} else {
			m_logger->log(LogLevel::Warning, "[PluginUIService_ImGui] Context set to nullptr");
		}
	}
}

void PluginUIService_ImGui::registerPluginUIRenderer(const std::string& pluginName, std::function<void()> renderCallback)
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);
	ensureInitialised();  // Initialize when first used
	m_pluginCallbacks[pluginName] = std::move(renderCallback);

	if (m_logger) {
		m_logger->log(LogLevel::Info, 
			std::format("[PluginUIService_ImGui] Registered UI for plugin: {}", pluginName));
	}
}

void PluginUIService_ImGui::unregisterPluginUIRenderer(const std::string& pluginName)
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);

	auto it = m_pluginCallbacks.find(pluginName);
	if (it != m_pluginCallbacks.end()) {
		m_pluginCallbacks.erase(it);

		if (m_logger) {
			m_logger->log(LogLevel::Info, 
				std::format("[PluginUIService_ImGui] Unregistered UI for plugin: {}", pluginName));
		}
	}
}

void PluginUIService_ImGui::renderPluginUIs()
{
	if (!isContextValid())
	{
		return;
	}

	std::lock_guard lock(m_callbackMutex);

	for (const auto& callback : m_pluginCallbacks | std::views::values)
	{
		if (callback)
		{
			callback();
		}
	}
}

bool PluginUIService_ImGui::isContextValid() const
{
	return m_context != nullptr;
}

std::string PluginUIService_ImGui::getDiagnosticInfo() const
{
	std::string info;

	info += std::format("PluginUIService_ImGui Diagnostics:\n");
	info += std::format("\tContext Valid: {}\n", isContextValid());
	info += std::format("\tStored Context: {}\n", static_cast<void*>(m_context));
	info += std::format("\tCurrent Context: {}\n", static_cast<void*>(ImGui::GetCurrentContext()));
	info += std::format("\tRegistered Plugins: {}\n", getRegisteredPluginCount());

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

size_t PluginUIService_ImGui::getRegisteredPluginCount() const
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);
	return m_pluginCallbacks.size();
}

void PluginUIService_ImGui::setErrorCallback(std::function<void(const std::string&)> callback)
{
	m_errorCallback = callback;

	if (m_logger) {
		m_logger->log(LogLevel::Debug, "[PluginUIService_ImGui] Error callback set");
	}
}

void PluginUIService_ImGui::ensureInitialised()
{
	if (!m_initialised && ImGui::GetCurrentContext()) 
	{
		// Do actual ImGui initialization here, not in constructor
		std::cout << "Initializing PluginUIService_ImGui on first use" << std::endl;
		m_initialised = true;
	}
}

bool PluginUIService_ImGui::isUIAvailable() const
{

	// Check if ImGui context is set and valid
	if (!m_context)
	{
		return false;
	}

	// Check if we're currently in a valid ImGui frame
	// (between NewFrame() and Render())
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	// Optionally check if ImGui is in a valid state for rendering
	ImGuiIO& io = ImGui::GetIO();
	if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
	{
		return false;
	}

	return true;
}