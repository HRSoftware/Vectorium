#pragma once
#include <any>
#include <chrono>
#include <functional>
#include <memory>
#include "IEngineUIBridge.h"
#include "Services/ServiceContainer.h"

class IUIService;

namespace spdlog
{
	class logger;
}

class SpdLogger;
class IPluginRESTService;
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

class Engine final : public IEngineUIBridge
{
public:

	Engine();
	~Engine() override;

	void init() const;
	bool shouldTick() const;
	void tick();
	void shutdown() const;

	[[nodiscard]] DataPacketRegistry* getDataPacketRegistry() const override;
	[[nodiscard]] PluginManager*      getPluginManager() const override;
	std::shared_ptr<ILogger>          getLogger() const override;
	std::shared_ptr<UILogSink>        getLogSink() const override;
	void                              enableEngineDebugLogging();
	void                              disableEngineDebugLogging();
	bool                              isEngineDebugLoggingEnabled() const;
	EngineSettings&                   getEngineSettings() override;
	void                              handleSettingChanged(const std::string& setting, const std::any& value);
	void                              updateLoggerFromSettings();
	void                              notifyUIInitialised() override;

	void                        setUIService(std::shared_ptr<IUIService> uiService) override;
	std::shared_ptr<IUIService> getUIService() const override;

private:

	EngineSettings     m_engineSetting;
	std::unique_ptr<PluginManager>      m_pPluginManager;
	std::unique_ptr<DataPacketRegistry> m_pDataPacketRegistry;
	std::shared_ptr<UILogSink>          m_pUiLogSink;

	//Services
	std::shared_ptr<IPluginRESTService> m_pRestClient;
	std::shared_ptr<ILogger> m_loggingService;
	std::shared_ptr<IUIService> m_uiService;


	std::chrono::high_resolution_clock::time_point m_lastUpdateTime;

	std::shared_ptr<spdlog::logger> m_combinedLogger; // TODO - remove coupling with SPDLog
	ServiceContainer m_serviceContainer;
};