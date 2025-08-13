
#include <cassert>
#include <format>
#include <utility>
#include "Plugin/PluginRuntimeContext.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/Logging/SpdLogger.h"

PluginRuntimeContext::PluginRuntimeContext(std::shared_ptr<ILogger> centralLogger, std::shared_ptr<DataPacketRegistry> registry, std::string pluginName)
: m_registry(std::move(registry))
, m_pluginName(std::move(pluginName))
{
	assert(centralLogger && "centralLogger is nullptr");
	scopedLogger = std::make_shared<SpdLogger>();

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

std::expected<std::shared_ptr<void>, std::string> PluginRuntimeContext::getServiceByTypeIndex(std::type_index tIdx)
{
	std::shared_lock lock(m_mutex);

	if (const auto it = m_services.find(tIdx); it != m_services.end() && it->second)
	{
		return it->second;
	}

	return std::unexpected("Service not found or unavailable");
}

bool PluginRuntimeContext::hasServiceByTypeIndex(std::type_index tIdx) const
{
	std::shared_lock lock(m_mutex);
	const auto it = m_services.find(tIdx);
	return it != m_services.end() && it->second != nullptr;
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