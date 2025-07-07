#pragma once

#include <functional>
#include "../Include/DataPacket/DataPacket.h"
#include "Logger/ILogger.h"

class IDataHandler;

/// <summary>
/// IPluginContext acts as a contract between the engine and a given plugin - what it can do, what other services it can access (ie handlers, loggers etc)
/// </summary>

struct IPluginContext
{
	virtual void registerHandler(std::type_index type, std::function<std::unique_ptr<IDataHandler>()>);
	virtual void unregisterHandler(std::type_index type);
	virtual      ~IPluginContext();
	virtual ILogger* getLogger() = 0;
};

inline void IPluginContext::registerHandler(std::type_index type, std::function<std::unique_ptr<IDataHandler>()>)
{
}

inline void IPluginContext::unregisterHandler(std::type_index type)
{
}

inline IPluginContext::~IPluginContext() = default;



/// <summary>
/// 
/// </summary>
struct IPlugin
{
	virtual void registerType(IPluginContext& context) = 0;
	virtual std::type_index getType() const = 0;
	virtual ~IPlugin() = default;
};