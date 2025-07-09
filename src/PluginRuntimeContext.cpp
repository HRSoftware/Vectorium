
#include <cassert>
#include <format>
#include <iostream>

#include "Logger/ComponentLogger.h"
#include "Plugin/PluginRuntimeContext.h"

PluginRuntimeContext::PluginRuntimeContext(std::shared_ptr<ILogger> centralLogger, std::string pluginName) : m_pluginName(pluginName)
{
	assert(centralLogger && "centralLogger is nullptr");
	scopedLogger = std::make_shared<ComponentLogger>(std::move(centralLogger), std::move(pluginName));
}

std::shared_ptr<ILogger> PluginRuntimeContext::getLoggerShared()
{
	return scopedLogger;
}

void PluginRuntimeContext::registerDataPacketHandler(const std::type_index type, std::shared_ptr<IDataPacketHandler> factory)
{
	pluginRegister[type] = std::move(factory);
	registeredTypes.insert(type);

	log(LogLevel::Info, std::format("Registered handle for {}", type.name()));
}

void PluginRuntimeContext::unregisterDataPacketHandler(const std::type_index type)
{
	pluginRegister.erase(type);
	registeredTypes.erase(type);

	log(LogLevel::Info, std::format("Unregistered handle for {}", type.name()));
}

std::string PluginRuntimeContext::getPluginName() const
{
	return m_pluginName;
}

void PluginRuntimeContext::log(LogLevel level, const std::string& message) const
{
	if(scopedLogger)
	{
		scopedLogger->log(level, message);
	}
}
