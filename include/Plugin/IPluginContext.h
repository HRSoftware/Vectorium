#pragma once

#include <format>

#include "../DataPacket/DataPacket.h"
#include "../DataPacket/IDataPacketHandler.h"
#include "Logger/ILogger.h"
#include "Logger/LogLevel.h"

class IDataPacketHandler;

/// <summary>
///		IPluginContext acts as a contract between the engine and a given plugin - what it can do, what other services it can access (ie handlers, loggers etc.)
///		It is used to give limited access to resources, or other plugins
/// </summary>

struct IPluginContext
{
	virtual void registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler>) = 0;
	virtual void log(LogLevel level, const std::string& message) const = 0;

	template <typename... Args>
	void logf(LogLevel level, std::string_view fmt, Args&&... args) const
	{
		log(level, std::vformat(fmt, std::make_format_args(args...)));
	}

	//virtual void unregisterDataPacketHandler(std::type_index type) = 0;

	virtual std::string getPluginName() const = 0;
	virtual std::shared_ptr<ILogger> getLoggerShared() = 0;
	virtual      ~IPluginContext() = default;
};
