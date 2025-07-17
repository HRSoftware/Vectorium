
#include <cassert>
#include <format>
#include <iostream>
#include <utility>

#include "Logger/ComponentLogger.h"
#include "Plugin/PluginRuntimeContext.h"

#include "DataPacket/DataPacketRegistry.h"

PluginRuntimeContext::PluginRuntimeContext(std::shared_ptr<ILogger> centralLogger, std::shared_ptr<DataPacketRegistry> registry, std::string pluginName)
: m_registry(std::move(registry))
, m_pluginName(pluginName)
{
	assert(centralLogger && "centralLogger is nullptr");
	scopedLogger = std::make_shared<ComponentLogger>(std::move(centralLogger), std::move(pluginName));

	assert(scopedLogger && "scopedLogger was nullptr");
}

std::shared_ptr<ILogger> PluginRuntimeContext::getLoggerShared()
{
	return scopedLogger;
}

bool PluginRuntimeContext::registerDataPacketHandler(const std::type_index type, std::shared_ptr<IDataPacketHandler> handler)
{
	if (!m_registry) return false;

	log(LogLevel::Info, std::format("Registered handle for {}", type.name()));
	return m_registry->registerDataPacketHandler(type, std::move(handler), m_pluginName);
}

void PluginRuntimeContext::dispatch(const DataPacket& packet)
{
	if (!packet.payload) return;

	m_registry->dispatch(packet);
}

//Needed? Move to base class?
void PluginRuntimeContext::unregisterDataPacketHandler()
{
	assert(m_registry && "Registry was nullptr!");
	if(m_registry)
	{
		m_registry->unregisterDataPacketHandlerForPlugin(m_pluginName);

		log(LogLevel::Info, std::format("Unregistered handles for {}", m_pluginName));
	}
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
