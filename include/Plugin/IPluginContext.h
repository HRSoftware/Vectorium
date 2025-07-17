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
	template<typename T>
	void registerTypedHandler(const std::shared_ptr<ITypedDataPacketHandler<T>>& handler)
	{
		auto adapter = std::make_shared<TypedDataPacketHandlerAdapter<T>>(handler);
		registerDataPacketHandler(typeid(T), adapter);
	}

	virtual void log(LogLevel level, const std::string& message) const = 0;

	template <typename... Args>
	void logf(LogLevel level, std::string_view fmt, Args&&... args)
	{
		if (getLoggerShared() && !getLoggerShared()->isDebugLoggingEnabled()) return;
		log(level, std::vformat(fmt, std::make_format_args(args...)));
	}

	//virtual void unregisterDataPacketHandler(std::type_index type) = 0;
	void enableDebugLogging()
	{
		if (!getLoggerShared()) return;
		return getLoggerShared()->enabledDebugLogging();
	}

	void disableDebugLogging()
	{
		if (!getLoggerShared()) return;
		return getLoggerShared()->disableDebugLogging();
	}

	bool isDebugLoggingEnabled()
	{
		if (!getLoggerShared()) return false;
		return getLoggerShared()->isDebugLoggingEnabled();
	}

	virtual std::string              getPluginName() const = 0;
	virtual std::shared_ptr<ILogger> getLoggerShared() = 0;
	virtual                          ~IPluginContext() = default;
	virtual void dispatch(const DataPacket& packet) = 0;

	protected:
		virtual bool registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> handler) = 0;
};

