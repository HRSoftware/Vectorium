#pragma once

#include <iostream>
#include <string>
#include "LogLevel.h"

/// <summary>
/// Defines an interface for logging messages with support for debug logging and plugin identification.
/// </summary>
class ILogger
{
	public:
	virtual ~ILogger()
	{
		std::cout << "ILogger destroyed\n";
	};
	virtual void log(LogLevel level, const std::string& message) = 0;

	virtual void enableDebugLogging() = 0;
	virtual void disableDebugLogging() = 0;

	virtual bool isDebugLoggingEnabled() const = 0;
	virtual void setPluginName(const std::string& name) = 0;
};
