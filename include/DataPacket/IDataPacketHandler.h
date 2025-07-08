#pragma once
#include "DataPacket.h"

/// <summary>
///		IDataPacketHandler is how the DataPacket is processes
/// </summary>
class IDataPacketHandler
{
public:
	virtual void handle(const DataPacket& packet) = 0;
	virtual ~IDataPacketHandler() = default;
};