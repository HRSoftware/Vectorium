#include "../include/DataPacket/DataPacketRegistry.h"
#include <string>
#include <utility>

DataPacketRegistry::DataPacketRegistry(std::shared_ptr<ILogger> log) : logger(std::move(log))
{
}

void DataPacketRegistry::registerDataPackets()
{
	dataPacketTypes = {typeid(int), typeid(float), typeid(std::string)};
}

const std::vector<std::type_index>& DataPacketRegistry::getDataPacketTypes() const
{
	return dataPacketTypes;
}