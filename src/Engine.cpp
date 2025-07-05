#include "../include/Engine.h"

#include <iostream>

void Engine::init()
{
	std::cout << "[Engine::init] - complete\n";
}

void Engine::tick()
{

}

void Engine::shutdown()
{
	std::cout << "[Engine::shutdown] - complete\n";
}

DataPacketRegistry& Engine::getDataPacketRegistry()
{
	return dataPacketRegistry;
}

PluginManager& Engine::getPluginManager()
{
	return pluginManager;
}