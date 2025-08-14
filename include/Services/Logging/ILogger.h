#pragma once

#include <string>
#include "LogLevel.h"

class ILogger
{
	public:
	virtual ~ILogger() = default;
	virtual void log(LogLevel level, const std::string& message) = 0;

	virtual void enableDebugLogging() = 0;
	virtual void disableDebugLogging() = 0;

	virtual bool isDebugLoggingEnabled() const = 0;
	virtual void setPluginName(const std::string& name) = 0;
};
