#include "Engine.h"

#include "../ui/UILogSink.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/Logging/SpdLogger.h"
#include "Plugin/PluginManager.h"
#include "Services/IServiceSpecialisations.h"


Engine::Engine()
{
	m_logger = std::make_shared<SpdLogger>();
	m_dataPacketRegistry = std::make_shared<DataPacketRegistry>(m_logger);

	PluginManager test(m_logger);
	//m_pluginManager = std::make_shared<PluginManager>(m_logger, m_dataPacketRegistry);

	m_logSink  = std::make_shared<UILogSink>();
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	m_combinedLogger = std::make_shared<spdlog::logger>("combined", spdlog::sinks_init_list{ std::move(consoleSink), m_logSink });

	spdlog::set_default_logger(m_combinedLogger);
	spdlog::set_level(spdlog::level::info);
}

Engine::~Engine() = default;

void Engine::init() const
{
	m_logger->log(LogLevel::Info, "[EngineInit] - Engine starting");

	m_pluginManager->init();

	m_logger->log(LogLevel::Info, "[Engine::init] - complete");
}

void Engine::tick() const
{
	m_pluginManager->tick();
}

void Engine::shutdown() const
{
	m_logger->log(LogLevel::Info, "[Engine::shutdown] - complete");
}

std::shared_ptr<DataPacketRegistry> Engine::getDataPacketRegistry() const
{
	return m_dataPacketRegistry;
}

std::shared_ptr<PluginManager> Engine::getPluginManager() const
{
	return m_pluginManager;
}

std::shared_ptr<ILogger> Engine::getLogger()
{
	return m_logger;
}

std::shared_ptr<UILogSink> Engine::getLogSink()
{
	return m_logSink;
}
