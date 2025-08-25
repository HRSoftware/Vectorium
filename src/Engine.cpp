#include "Engine.h"

#include <iostream>
#include <utility>

#include "../ui/include/Services/UI/PluginUIService_ImGui.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/Logging/SpdLogger.h"
#include "Plugin/PluginManager.h"
#include "Services/Logging/UILogSink.h"
#include "Services/REST/PluginRESTService_Cpr.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

Engine::Engine()
{
	auto consoleSink   = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	m_pUiLogSink = std::make_shared<UILogSink>();
	m_pUiLogSink->set_level(spdlog::level::debug);

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(consoleSink);
	sinks.push_back(m_pUiLogSink);

	m_loggingService = std::make_shared<SpdLogger>("Engine", sinks);

	m_pDataPacketRegistry = std::make_unique<DataPacketRegistry>(*m_loggingService);
	m_pRestClient = std::make_shared<PluginRESTService_Cpr>();

	m_serviceContainer.registerService(m_loggingService);
	m_serviceContainer.registerService(m_pRestClient);

	m_pPluginManager = std::make_unique<PluginManager>(*m_loggingService
		, *m_pDataPacketRegistry
		, m_pUiLogSink
		, m_serviceContainer);

	spdlog::set_level(spdlog::level::info);

	m_engineSetting.onSettingChanged = [this](const std::string& setting, const std::any& value)
	{
		handleSettingChanged(setting, value);
	};
}

Engine::~Engine()
{
	// This order so nothing crashes
	m_pPluginManager.reset();
	m_pDataPacketRegistry.reset();
	m_pRestClient.reset();
	m_loggingService.reset();
}

void Engine::init() const
{
	m_loggingService->log(LogLevel::Info, "[Engine::init] - Engine starting");

	m_pPluginManager->init();

	m_loggingService->log(LogLevel::Info, "[Engine::init] - complete");
}

void EngineSettings::setDebugLogging(bool enabled)
{
	//issue with logging be stuck to enabled on the UI
	if(m_debugLoggingEnabled != enabled)
	{
		m_debugLoggingEnabled = enabled;
		if(onSettingChanged)
		{
			onSettingChanged("debugLogging", enabled);
		}
	}
}

bool EngineSettings::isDebugLoggingEnabled() const
{
	return m_debugLoggingEnabled;
}

bool Engine::shouldTick() const
{
	if((std::chrono::high_resolution_clock::now() - m_lastUpdateTime >= m_engineSetting.getPluginUpdateInterval()))
	{
		return true;
	}

	return false;
}

void Engine::tick()
{
	//If we haven't polling for over a second
	if(shouldTick())
	{
		m_loggingService->log(LogLevel::Debug, "Engine Tick");
		m_pPluginManager->tick();
		m_lastUpdateTime = std::chrono::high_resolution_clock::now();
	}
}

void Engine::shutdown() const
{
	m_loggingService->log(LogLevel::Info, "[Engine::shutdown] - complete");
}

DataPacketRegistry* Engine::getDataPacketRegistry() const
{
	return m_pDataPacketRegistry.get();
}

PluginManager* Engine::getPluginManager() const
{
	return m_pPluginManager.get();
}

std::shared_ptr<ILogger> Engine::getLogger() const
{
	return m_loggingService;
}

std::shared_ptr<UILogSink> Engine::getLogSink() const
{
	return m_pUiLogSink;
}

void Engine::enableEngineDebugLogging()
{
	if(m_loggingService)
	{
		m_loggingService->enableDebugLogging();
		m_loggingService->log(LogLevel::Debug, "Engine debug logging enabled");
	}
}

void Engine::disableEngineDebugLogging()
{
	if(m_loggingService)
	{
		m_loggingService->log(LogLevel::Debug, "Engine debug logging disabled");
		m_loggingService->disableDebugLogging();
	}
}

bool Engine::isEngineDebugLoggingEnabled() const
{
	if(m_loggingService)
	{
		return m_loggingService->isDebugLoggingEnabled();
	}

	return false;
}

EngineSettings& Engine::getEngineSettings()
{
	return m_engineSetting;
}

void Engine::handleSettingChanged(const std::string& setting, const std::any& value)
{
	if (setting == "debugLogging")
	{
		bool bShouldEnabledDebugLogging = std::any_cast<bool>(value);
		m_loggingService->log(LogLevel::Info, std::format("Engine debug logging {}", bShouldEnabledDebugLogging ? "enabled" : "disabled"));

		bShouldEnabledDebugLogging
		? m_loggingService->enableDebugLogging()
		: m_loggingService->disableDebugLogging();
	}
}

void Engine::updateLoggerFromSettings()
{
	// Sync logger state with current settings
	if (m_engineSetting.isDebugLoggingEnabled())
	{
		m_loggingService->enableDebugLogging();
	}
	else
	{
		m_loggingService->disableDebugLogging();
	}
}

void Engine::notifyUIInitialised()
{
	m_loggingService->log(LogLevel::Info, "Received notification of UI initialisation");
}

void Engine::setUIService(std::shared_ptr<IPluginUIService> uiService)
{
	m_uiService = std::move(uiService);
	if (m_uiService)
	{
		m_serviceContainer.registerService<IPluginUIService>(m_uiService);
		m_loggingService->log(LogLevel::Info, "UI service registered with engine");
	}
}

std::shared_ptr<IPluginUIService> Engine::getUIService() const
{
	return m_uiService;
}
