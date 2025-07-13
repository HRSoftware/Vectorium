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
	void tick() const;
	void shutdown() const;

	[[nodiscard]] std::shared_ptr<DataPacketRegistry> getDataPacketRegistry() const;
	[[nodiscard]] std::shared_ptr<PluginManager> getPluginManager() const;
	std::shared_ptr<ILogger>     getLogger();

private:
	std::shared_ptr<PluginManager> m_pluginManager;
	std::shared_ptr<DataPacketRegistry> m_dataPacketRegistry;
	std::shared_ptr<ILogger> m_logger;
};