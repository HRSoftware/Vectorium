#pragma once

#include <set>

#include "IPlugin.h"
#include "Logger/ILogger.h"

class PluginRuntimeContext : public IPluginContext
{
public:
	PluginRuntimeContext(std::shared_ptr<ILogger> centralLogger, std::string pluginName);

	std::shared_ptr<ILogger> getLoggerShared() override;

	void        registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> factory) override;
	void        unregisterDataPacketHandler(std::type_index type);
	std::string getPluginName() const override;
	void        log(LogLevel level, const std::string& message) const override;

private:
	std::unordered_map<std::type_index, std::shared_ptr<IDataPacketHandler>> pluginRegister;
	std::set<std::type_index> registeredTypes;
	std::shared_ptr<ILogger> scopedLogger;
	std::string m_pluginName;
};