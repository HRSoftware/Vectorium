#pragma once
#include <memory>
#include <typeindex>
#include <vector>

class ILogger;

/// <summary>
/// DataPacketRegistry tracks all packet types, and the handlers defined for them
/// </summary>
class DataPacketRegistry
{
public:
	DataPacketRegistry(std::shared_ptr<ILogger> log);

	void                                registerDataPackets();
	const std::vector<std::type_index>& getDataPacketTypes() const;



private:
	std::vector<std::type_index> dataPacketTypes;
	std::shared_ptr<ILogger> logger;
};
