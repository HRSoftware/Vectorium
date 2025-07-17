#pragma once


#include "Logger/ILogger.h"
#include <memory>
#include <string>
#include <cassert>
#include <iostream>


class LoggablePlugin
{
public:
	LoggablePlugin() = default;
	virtual ~LoggablePlugin() = default;

	void setLogger(std::shared_ptr<ILogger> logger, std::string name = "Unknown");
	void unsetLogger();

	void enableDebugLogging();
	void disableDebugLogging();
	bool isDebugLoggingEnabled() const;

protected:
	void log(LogLevel level, const std::string& msg) const;

private:
	std::shared_ptr<ILogger> m_logger;
	std::string m_componentName;
};