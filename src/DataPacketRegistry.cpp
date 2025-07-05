#include "../include/DataPacket/DataPacketRegistry.h"
#include <string>

void DataPacketRegistry::registerDataPackets()
{
	dataPacketTypes = {typeid(int), typeid(float), typeid(std::string)};
}

const std::vector<std::type_index>& DataPacketRegistry::getDataPacketTypes() const
{
	return dataPacketTypes;
}