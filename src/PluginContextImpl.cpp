
#include "../include/Plugin/PluginContextImpl.h"

#include <iostream>
#include <format>

#include "Logger/ComponentLogger.h"

PluginContextImpl::PluginContextImpl(std::shared_ptr<ILogger> centralLogger, std::string pluginName)
{
	scopedLogger = std::make_shared<ComponentLogger>(std::move(centralLogger), std::move(pluginName));
	
}

ILogger* PluginContextImpl::getLogger()
{
	return scopedLogger.get();
}

std::shared_ptr<ILogger> PluginContextImpl::getLoggerShared()
{
	return scopedLogger;
}

void PluginContextImpl::registerHandler(const std::type_index type, std::function<std::unique_ptr<IDataHandler>()> factory)
{
	pluginRegister[type] = std::move(factory);
	registeredTypes.insert(type);
	std::cout << std::format("Registered handle for {}\n", type.name());

}

void PluginContextImpl::unregisterHandler(const std::type_index type)
{
	pluginRegister.erase(type);
	registeredTypes.erase(type);
	std::cout << std::format("Unregistered handle for {}\n", type.name());
}
