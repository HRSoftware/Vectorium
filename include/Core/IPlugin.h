#pragma once

#include <string>
#include <functional>
#include "DataPacket.h"

class IDataHandler;

// Keep the registry of plugins and the actual plugin decoupled
struct IPluginContext
{
	virtual void registerHandler(std::type_index type, std::function<std::unique_ptr<IDataHandler>()>);
	virtual void unregisterHandler(std::type_index type);
	virtual      ~IPluginContext();
};

inline void IPluginContext::registerHandler(std::type_index type, std::function<std::unique_ptr<IDataHandler>()>)
{
}

inline void IPluginContext::unregisterHandler(std::type_index type)
{
}

inline IPluginContext::~IPluginContext() = default;




struct IPlugin
{
	virtual void registerType(IPluginContext& context) = 0;
	virtual std::type_index getType() const = 0;
	virtual ~IPlugin() = default;
};