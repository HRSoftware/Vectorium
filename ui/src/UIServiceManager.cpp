#include "UI/UIServiceManager.h"
#include "UI/ImGuiContextManager.h"
#include "Services/UI/PluginUIService_ImGui.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include <format>

UIServiceManager::UIServiceManager(std::shared_ptr<ILogger> logger)
	: m_logger(std::move(logger))
	, m_initialised(false)
{
	logInfo("UIServiceManager created");
}

UIServiceManager::~UIServiceManager()
{
	shutdown();
	logInfo("UIServiceManager destroyed");
}

bool UIServiceManager::initialise()
{
	if (m_initialised)
	{
		logDebug("UIServiceManager already initialized");
		return true;
	}

	// Create context manager
	m_contextManager = std::make_unique<ImGuiContextManager>(m_logger);
	if (!m_contextManager->initialise())
	{
		logError("Failed to initialize ImGui context manager");
		return false;
	}

	// Create ImGui service
	m_imguiService = std::make_shared<PluginUIService_ImGui>(m_logger);
	m_imguiService->setImGuiContext(m_contextManager->getContext());

	m_initialised = true;
	logInfo("UIServiceManager initialized successfully");
	return true;
}

void UIServiceManager::shutdown()
{
	if (!m_initialised)
	{
		return;
	}

	logInfo("Shutting down UIServiceManager");

	// Shutdown in reverse order
	if (m_imguiService)
	{
		m_imguiService.reset();
	}

	if (m_contextManager)
	{
		m_contextManager->shutdown();
		m_contextManager.reset();
	}

	m_initialised = false;
	logInfo("UIServiceManager shutdown complete");
}

void UIServiceManager::updateMainContext(ImGuiContext* context) const
{
	if (m_contextManager)
	{
		m_contextManager->setContext(context);
	}

	if (m_imguiService)
	{
		m_imguiService->setImGuiContext(context);
	}

	logInfo(std::format("Updated main context to: {}", static_cast<void*>(context)));
}

void UIServiceManager::renderPluginUIs() const
{
	if (!m_initialised || !m_contextManager)
	{
		return;
	}

	m_contextManager->renderAllPluginUIs();
}

std::shared_ptr<IPluginUIService> UIServiceManager::getUIService()
{
	return m_imguiService;
}

ImGuiContextManager* UIServiceManager::getContextManager() const
{
	return m_contextManager.get();
}

bool UIServiceManager::isInitialised() const
{
	return m_initialised;
}

std::string UIServiceManager::getStatusInfo() const
{
	if (!m_initialised)
	{
		return "UIServiceManager: Not initialized";
	}

	std::string info = "UIServiceManager: Initialized\n";

	if (m_contextManager)
	{
		info += "  " + m_contextManager->getContextInfo() + "\n";
		info += std::format("  Registered plugins: {}\n", m_contextManager->getRegisteredPluginCount());
	}

	if (m_imguiService)
	{
		info += std::format("  ImGui service: {}\n", m_imguiService->isContextValid() ? "Valid" : "Invalid");
	}

	return info;
}

void UIServiceManager::logError(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Error, std::format("[UIServiceManager] {}", message));
	}
}

void UIServiceManager::logInfo(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Info, std::format("[UIServiceManager] {}", message));
	}
}

void UIServiceManager::logDebug(const std::string& message) const
{
	if (m_logger)
	{
		m_logger->log(LogLevel::Debug, std::format("[UIServiceManager] {}", message));
	}
}