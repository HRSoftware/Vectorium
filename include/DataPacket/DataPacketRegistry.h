#pragma once
#include <memory>
#include <typeindex>
#include <vector>
#include <set>
#include <unordered_map>

class IDataPacketHandler;

class ILogger;
class DataPacket;

/// <summary>
/// DataPacketRegistry knows about all packet types, and the handlers subscribed to each
/// </summary>
class DataPacketRegistry
{
public:
	DataPacketRegistry(std::shared_ptr<ILogger> log);

	bool registerDataPacketHandler(std::type_index packetType, std::shared_ptr<IDataPacketHandler> handler);

	bool registerWildcardHandler(std::shared_ptr<IDataPacketHandler> handler);
	const std::vector<std::type_index>& getDataPacketTypes() const;

	void dispatch(const DataPacket& packet);


private:
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<IDataPacketHandler>>> m_handlers;
	std::vector<std::shared_ptr<IDataPacketHandler>> m_wildcardHandlers;
	
	std::shared_ptr<ILogger> logger;
};
