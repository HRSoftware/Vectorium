#pragma once

#include <functional>
#include "../DataPacket/DataPacket.h"
#include "../DataPacket/IDataPacketHandler.h"
#include "Logger/ILogger.h"

class IDataPacketHandler;

/// <summary>
///		IPluginContext acts as a contract between the engine and a given plugin - what it can do, what other services it can access (ie handlers, loggers etc.)
/// </summary>

struct IPluginContext
{
	virtual void registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler>);
	virtual void unregisterDataPacketHandler(std::type_index type);
	virtual      ~IPluginContext();
	virtual std::shared_ptr<ILogger> getLogger() = 0;
};

inline void IPluginContext::registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler>)
{
}

inline void IPluginContext::unregisterDataPacketHandler(std::type_index type)
{
}

inline IPluginContext::~IPluginContext() = default;



/// <summary>
/// 
/// </summary>
struct IPlugin
{
	virtual void onPluginLoad(IPluginContext& context) = 0;
	virtual void onPluginUnload() = 0;
	virtual std::type_index getType() const = 0;
	virtual ~IPlugin() = default;
};