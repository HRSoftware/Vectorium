#pragma once

#include "../include/DataPacket/DataPacketRegistry.h"
#include "../include/Plugin/PluginManager.h"

class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(PluginManager& pm, DataPacketRegistry& dpr);

	void drawPluginUI();
	void drawDataPacketUI();

private:
	PluginManager& pluginManager;
	DataPacketRegistry& dataPacketRegistry;
};
