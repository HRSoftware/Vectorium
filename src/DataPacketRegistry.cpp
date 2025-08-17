#include "DataPacket/DataPacketRegistry.h"

#include <cassert>
#include <format>
#include <ranges>
#include <string>
#include <utility>
#include "DataPacket/DataPacket.h"
#include "DataPacket/IDataPacketHandler.h"
#include "Services/Logging/LogLevel.h"

DataPacketRegistry::DataPacketRegistry(ILogger& log) : m_logger(log)
{

}

bool DataPacketRegistry::registerDataPacketHandler(std::type_index packetType, std::shared_ptr<IDataPacketHandler> handler, const std::string& pluginName)
{
	m_handlers[packetType].emplace_back(std::move(handler), pluginName);
	return true;
}

std::vector<std::type_index> DataPacketRegistry::getDataPacketTypes() const
{
	std::vector<std::type_index> types;
	types.reserve(m_handlers.size());

	for(const auto& key : m_handlers | std::views::keys)
	{
		types.push_back(key);
	}

	return types;
}

bool DataPacketRegistry::registerWildcardHandler(std::shared_ptr<IDataPacketHandler> handler, const std::string& pluginName)
{
	m_wildcardHandlers.emplace_back(std::move(handler), pluginName);
    return true;
}

void DataPacketRegistry::unregisterDataPacketHandlerForPlugin(std::string_view pluginName)
{
	for (auto it = m_handlers.begin(); it != m_handlers.end(); )
	{
		auto& handlerList = it->second;

		// Remove all handlers matching pluginName
		std::erase_if(handlerList,
		              [&](const auto& pair)
		              {
			              return pair.second == pluginName;
		              });

		// Clean up empty type entries
		if (handlerList.empty())
		{
			it = m_handlers.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void DataPacketRegistry::dispatch(const DataPacket &packet)
{
	assert(packet.payloadType != std::type_index(typeid(void)) && "Invalid payloadType!");

	const auto& it = m_handlers.find(packet.payloadType);
	if(it != m_handlers.end())
	{
		for(const auto& key : it->second | std::views::keys)
		{
			if(!key) continue;

			key->handle(packet);
		}
	}

	for(const auto& key : m_wildcardHandlers | std::views::keys)
	{
		if(!key) continue;

		key->handle(packet);
	}
}
