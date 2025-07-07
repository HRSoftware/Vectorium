#pragma once
#include "DataPacket/DataPacketRegistry.h"
#include "Plugin/PluginManager.h"
#include "Logger/SpdLogger.h"

class Engine
{
public:

	Engine();
	~Engine();

	void init() const;
	void tick();
	void shutdown() const;

	DataPacketRegistry&      getDataPacketRegistry() const;
	PluginManager&           getPluginManager() const;
	std::shared_ptr<ILogger> getLogger();

private:
	std::unique_ptr<PluginManager> pluginManager;
	std::unique_ptr<DataPacketRegistry> dataPacketRegistry;
	std::shared_ptr<ILogger> logger;
};