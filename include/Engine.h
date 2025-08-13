#pragma once
#include <memory>

#include "spdlog/logger.h"

class ILogger;
class UILogSink;
class PluginManager;
class DataPacketRegistry;


class Engine
{
public:

	Engine();
	~Engine();

	void init() const;
	void tick() const;
	void shutdown() const;

	[[nodiscard]] std::shared_ptr<DataPacketRegistry> getDataPacketRegistry() const;
	[[nodiscard]] std::shared_ptr<PluginManager>      getPluginManager() const;
	std::shared_ptr<ILogger>                          getLogger();
	std::shared_ptr<UILogSink>                        getLogSink();

private:
	std::shared_ptr<PluginManager>      m_pluginManager;
	std::shared_ptr<DataPacketRegistry> m_dataPacketRegistry;
	std::shared_ptr<ILogger>            m_logger;
	std::shared_ptr<UILogSink>          m_logSink;


	std::shared_ptr<spdlog::logger>     m_combinedLogger; // TODO - remove coupling with SPDLog
};