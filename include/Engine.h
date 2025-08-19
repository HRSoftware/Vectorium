#pragma once
#include <any>
#include <memory>
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class SpdLogger;
class IRestClient;
class ILogger;
class UILogSink;
class PluginManager;
class DataPacketRegistry;

class EngineSettings final
{
	private:
		bool m_debugLoggingEnabled = false;
		bool m_paused = false;
		int m_maxPlugins = 50;
		std::chrono::seconds m_pluginUpdateInterval{1};

	public:
		void setDebugLogging(bool enabled);
		bool isDebugLoggingEnabled() const;

		// Engine state
		bool isPaused() const
		{
			return m_paused;
		}

		void setPaused(bool paused)
		{
			m_paused = paused;
		}

		// Plugin settings
		int getMaxPlugins() const
		{ return m_maxPlugins;
		}

		void setMaxPlugins(int max)
		{
			m_maxPlugins = max;
		}

		std::chrono::seconds getPluginUpdateInterval() const
		{
			return m_pluginUpdateInterval;
		}

		void setPluginUpdateInterval(std::chrono::seconds interval)
		{
			m_pluginUpdateInterval = interval;
		}

		std::function<void(const std::string& settingName, const std::any& newValue)> onSettingChanged;
};

class Engine final
{
public:

	Engine();
	~Engine();

	void init() const;
	bool shouldTick() const;
	void tick();
	void shutdown() const;

	[[nodiscard]] DataPacketRegistry* getDataPacketRegistry() const;
	[[nodiscard]] PluginManager*      getPluginManager() const;
	std::shared_ptr<ILogger>          getLogger();
	std::shared_ptr<UILogSink>        getLogSink() const;
	void                              enableEngineDebugLogging();
	void                              disableEngineDebugLogging();
	bool                              isEngineDebugLoggingEnabled() const;
	EngineSettings&                   getEngineSettings();
	void                              handleSettingChanged(const std::string& setting, const std::any& value);
	void                              updateLoggerFromSettings();

private:

	EngineSettings     m_engineSetting;
	std::unique_ptr<PluginManager>      m_pPluginManager;
	std::unique_ptr<DataPacketRegistry> m_pDataPacketRegistry;
	std::shared_ptr<UILogSink>          m_pUiLogSink;

	//Services
	std::shared_ptr<IRestClient> m_pRestClient;
	std::shared_ptr<ILogger> m_engineLogger;


	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

	std::shared_ptr<spdlog::logger>     m_combinedLogger; // TODO - remove coupling with SPDLog
};