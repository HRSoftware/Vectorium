#include "../include/Engine.h"
#include "Logger/SpdLogger.h"

#include <iostream>

Engine::Engine()
{
	m_logger = std::make_shared<SpdLogger>();
	m_dataPacketRegistry = std::make_unique<DataPacketRegistry>(m_logger);
	m_pluginManager = std::make_unique<PluginManager>(m_logger, m_dataPacketRegistry);
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
