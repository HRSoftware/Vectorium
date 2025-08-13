#pragma once


#include "Services/Logging/ILogger.h"

class SpdLogger : public ILogger
{
public:
	void log(LogLevel level, const std::string& msg) override;
	void enableDebugLogging() override;
	void disableDebugLogging() override;
	bool isDebugLoggingEnabled() const override;
	void setPluginName(const std::string& name) override;
};