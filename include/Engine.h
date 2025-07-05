#pragma once
#include "DataPacket\DataPacketRegistry.h"
#include "Plugin\PluginManager.h"

class Engine
{
public:

	void init();
	void tick();
	void shutdown();

	DataPacketRegistry& getDataPacketRegistry();
	PluginManager& getPluginManager();

private:
	PluginManager pluginManager;
	DataPacketRegistry dataPacketRegistry;
};