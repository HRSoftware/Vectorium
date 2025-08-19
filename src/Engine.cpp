#include "Engine.h"

#include <iostream>

#include "DataPacket/DataPacketRegistry.h"
#include "Services/Logging/SpdLogger.h"
#include "Plugin/PluginManager.h"
#include "Services/REST/RestClient_HttpLib.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Services/Logging/UILogSink.h"
#include "Services/REST/RestClient_Cpr.h"
#include "spdlog/spdlog.h"


Engine::Engine()
{
	auto consoleSink   = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	m_pUiLogSink = std::make_shared<UILogSink>();
	m_pUiLogSink->set_level(spdlog::level::debug);

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(consoleSink);
	sinks.push_back(m_pUiLogSink);

	m_engineLogger = std::make_shared<SpdLogger>("Engine", sinks);

	m_pDataPacketRegistry = std::make_unique<DataPacketRegistry>(*m_engineLogger);
	m_pRestClient         = std::make_unique<RESTClient_Cpr>();
	m_pPluginManager      = std::make_unique<PluginManager>(*m_engineLogger, *m_pDataPacketRegistry, m_pRestClient, m_pUiLogSink);

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
	m_engineLogger.reset();
}

void Engine::init() const
{
	m_engineLogger->log(LogLevel::Info, "[Engine::init] - Engine starting");

	m_pPluginManager->init();

	m_engineLogger->log(LogLevel::Info, "[Engine::init] - complete");
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
		m_engineLogger->log(LogLevel::Debug, "Engine Tick");
		m_pPluginManager->tick();
		m_lastUpdateTime = std::chrono::high_resolution_clock::now();
	}
}

void Engine::shutdown() const
{
	m_engineLogger->log(LogLevel::Info, "[Engine::shutdown] - complete");
}

DataPacketRegistry* Engine::getDataPacketRegistry() const
{
	return m_pDataPacketRegistry.get();
}

PluginManager* Engine::getPluginManager() const
{
	return m_pPluginManager.get();
}

std::shared_ptr<ILogger> Engine::getLogger()
{
	return m_engineLogger;
}

std::shared_ptr<UILogSink> Engine::getLogSink() const
{
	return m_pUiLogSink;
}

void Engine::enableEngineDebugLogging()
{
	if(m_engineLogger)
	{
		m_engineLogger->enableDebugLogging();
		m_engineLogger->log(LogLevel::Debug, "Engine debug logging enabled");
	}
}

void Engine::disableEngineDebugLogging()
{
	if(m_engineLogger)
	{
		m_engineLogger->log(LogLevel::Debug, "Engine debug logging disabled");
		m_engineLogger->disableDebugLogging();
	}
}

bool Engine::isEngineDebugLoggingEnabled() const
{
	if(m_engineLogger)
	{
		return m_engineLogger->isDebugLoggingEnabled();
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
		bool enabled = std::any_cast<bool>(value);
		m_engineLogger->log(LogLevel::Info, std::format("Engine debug logging {}", enabled ? "enabled" : "disabled"));

		if (enabled)
		{
			m_engineLogger->enableDebugLogging();
		} else
		{
			m_engineLogger->disableDebugLogging();
		}
	}
}

void Engine::updateLoggerFromSettings()
{
	// Sync logger state with current settings
	if (m_engineSetting.isDebugLoggingEnabled())
	{
		m_engineLogger->enableDebugLogging();
	}
	else
	{
		m_engineLogger->disableDebugLogging();
	}
}
