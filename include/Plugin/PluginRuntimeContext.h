#pragma once

#include <set>

#include "IPlugin.h"
#include "Logger/ILogger.h"

class DataPacketRegistry;

class PluginRuntimeContext : public IPluginContext
{
public:
	PluginRuntimeContext(std::shared_ptr<ILogger> centralLogger
						,std::shared_ptr<DataPacketRegistry> registry
						,std::string pluginName);

	std::shared_ptr<ILogger> getLoggerShared() override;

	void        unregisterDataPacketHandler();
	std::string getPluginName() const override;
	void        log(LogLevel level, const std::string& message) const override;

private:
	std::shared_ptr<DataPacketRegistry> m_registry;
	std::shared_ptr<ILogger> scopedLogger;
	std::string m_pluginName;

	protected:
		bool registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> handler) override;

public:
	void dispatch(const DataPacket& packet) override;
};
