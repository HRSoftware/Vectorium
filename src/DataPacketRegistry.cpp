#include "DataPacket/DataPacketRegistry.h"
#include <string>
#include <utility>
#include "DataPacket/DataPacket.h"
#include "DataPacket/IDataPacketHandler.h"


DataPacketRegistry::DataPacketRegistry(std::shared_ptr<ILogger> log) : logger(std::move(log))
{

}

bool DataPacketRegistry::registerDataPacketHandler(std::type_index packetType, std::shared_ptr<IDataPacketHandler> handler)
{
	
	m_handlers[packetType].push_back(std::move(handler));
	return true;
}

const std::vector<std::type_index> &DataPacketRegistry::getDataPacketTypes() const
{
	return {};
	//return dataPacketTypes;
}

bool DataPacketRegistry::registerWildcardHandler(std::shared_ptr<IDataPacketHandler> handler)
{
	m_wildcardHandlers.push_back(std::move(handler));
    return true;
}

void DataPacketRegistry::dispatch(const DataPacket &packet)
{
	auto it = m_handlers.find(packet.payloadType);
	if(it != m_handlers.end())
	{
		for(const auto& handler : it->second)
		{
			if(!handler) continue;
			
			handler->handle(packet);
		}
	}

	for(const auto& handler : m_wildcardHandlers)
	{
		if(!handler) continue;
		
		handler->handle(packet);
	}
}