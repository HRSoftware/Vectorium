#pragma once
#include "DataPacket.h"

// The idea is that all datapackets type will be added via their own plugin. This plugin will also register the handler for this type.
class IDataHandler
{
public:
	virtual void handle(const DataPacket& packet) = 0;
	virtual ~IDataHandler() = default;
};