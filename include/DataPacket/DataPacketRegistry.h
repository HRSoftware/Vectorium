#pragma once
#include <typeindex>
#include <vector>


/// <summary>
/// DataPacketRegistry tracks all packet types, and the handlers defined for them
/// </summary>
class DataPacketRegistry
{
public:
	void                                registerDataPackets();
	const std::vector<std::type_index>& getDataPacketTypes() const;

private:
	std::vector<std::type_index> dataPacketTypes;
};
