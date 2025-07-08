#pragma once

#include <set>

#include "IPlugin.h"
#include "Logger/ILogger.h"

class PluginContextImpl : public IPluginContext
{
public:
	PluginContextImpl(std::shared_ptr<ILogger> centralLogger, std::string pluginName);

	std::shared_ptr<ILogger> getLogger() override;
	std::shared_ptr<ILogger> getLoggerShared();

	void registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> factory) override;
	void unregisterDataPacketHandler(std::type_index type) override;

private:
	std::unordered_map<std::type_index, std::shared_ptr<IDataPacketHandler>> pluginRegister;
	std::set<std::type_index> registeredTypes;
	std::shared_ptr<ILogger> scopedLogger;
};