#include "../include/Engine.h"
#include "Logger/SpdLogger.h"

#include <iostream>

Engine::Engine()
{
	logger = std::make_shared<SpdLogger>();
	pluginManager = std::make_unique<PluginManager>(logger);
	dataPacketRegistry = std::make_unique<DataPacketRegistry>(logger);
}

Engine::~Engine() = default;

void Engine::init() const
{
	logger->log(LogLevel::Info, "[EngineInit] - Engine starting");

	pluginManager->scanPluginsFolder();

	logger->log(LogLevel::Info, "[Engine::init] - complete");
}

void Engine::tick()
{

}

void Engine::shutdown() const
{
	logger->log(LogLevel::Info, "[Engine::shutdown] - complete");
}

DataPacketRegistry& Engine::getDataPacketRegistry() const
{
	return *dataPacketRegistry;
}

PluginManager& Engine::getPluginManager() const
{
	return *pluginManager;
}

std::shared_ptr<ILogger> Engine::getLogger()
{
	return logger;
}
