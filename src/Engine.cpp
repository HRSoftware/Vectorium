#include "Engine.h"
#include "../ui/UILogSink.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/Logging/SpdLogger.h"
#include "Plugin/PluginManager.h"
#include "Services/IServiceSpecialisations.h"
#include "Services/REST/RestClient_HttpLib.h"

Engine::Engine()
{
	m_pLogger = std::make_shared<SpdLogger>();
	m_pDataPacketRegistry = std::make_shared<DataPacketRegistry>(m_pLogger);
	m_pPluginManager = std::make_shared<PluginManager>(m_pLogger, m_pDataPacketRegistry);
	m_pRestClient = std::make_shared<RESTClient_HttpLib>();
	m_pLogSink  = std::make_shared<UILogSink>();

	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	m_combinedLogger = std::make_shared<spdlog::logger>("combined", spdlog::sinks_init_list{ std::move(consoleSink), m_pLogSink });

	spdlog::set_default_logger(m_combinedLogger);
	spdlog::set_level(spdlog::level::info);
}

Engine::~Engine() = default;

void Engine::init() const
{
	m_pLogger->log(LogLevel::Info, "[Engine::init] - Engine starting");

	m_pPluginManager->init();

	m_pLogger->log(LogLevel::Info, "[Engine::init] - complete");
}

void Engine::tick() const
{
	m_pPluginManager->tick();
}

void Engine::shutdown() const
{
	m_pLogger->log(LogLevel::Info, "[Engine::shutdown] - complete");
}

std::shared_ptr<DataPacketRegistry> Engine::getDataPacketRegistry() const
{
	return m_pDataPacketRegistry;
}

std::shared_ptr<PluginManager> Engine::getPluginManager() const
{
	return m_pPluginManager;
}

std::shared_ptr<ILogger> Engine::getLogger()
{
	return m_pLogger;
}

std::shared_ptr<UILogSink> Engine::getLogSink()
{
	return m_pLogSink;
}
