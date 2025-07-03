#pragma once

#include <set>

#include "IPlugin.h"

class PluginContextImpl : public IPluginContext
{
public:
	void registerHandler(std::type_index type, std::function<std::unique_ptr<IDataHandler>()> factory) override;
	void unregisterHandler(std::type_index type) override;

private:
	std::unordered_map<std::type_index, std::function<std::unique_ptr<IDataHandler>()>> pluginRegister;
	std::set<std::type_index> registeredTypes;
};