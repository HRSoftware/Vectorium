
#include <cassert>
#include <format>
#include <iostream>

#include "Logger/ComponentLogger.h"
#include "Plugin/PluginContextImpl.h"

PluginContextImpl::PluginContextImpl(std::shared_ptr<ILogger> centralLogger, std::string pluginName)
{
	assert(centralLogger && "centralLogger is nullptr");
	scopedLogger = std::make_shared<ComponentLogger>(std::move(centralLogger), std::move(pluginName));
}

std::shared_ptr<ILogger> PluginContextImpl::getLogger()
{
	return scopedLogger;
}

std::shared_ptr<ILogger> PluginContextImpl::getLoggerShared()
{
	return scopedLogger;
}

void PluginContextImpl::registerDataPacketHandler(const std::type_index type, std::shared_ptr<IDataPacketHandler> factory)
{
	pluginRegister[type] = std::move(factory);
	registeredTypes.insert(type);

	std::cout << std::format("Registered handle for {}\n", type.name());
}

void PluginContextImpl::unregisterDataPacketHandler(const std::type_index type)
{
	pluginRegister.erase(type);
	registeredTypes.erase(type);

	std::cout << std::format("Unregistered handle for {}\n", type.name());

	scopedLogger->log(LogLevel::Info,"");
}
