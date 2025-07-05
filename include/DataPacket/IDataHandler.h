#pragma once
#include "DataPacket.h"

/// <summary>
/// IDataHandler is how the DataPacket is processes
/// </summary>
class IDataHandler
{
public:
	virtual void handle(const DataPacket& packet) = 0;
	virtual ~IDataHandler() = default;
};