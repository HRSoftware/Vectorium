#pragma once
#include "ILogger.h"
#include <memory>

class SpdLogger;

/// <summary>
/// Provides logging functionality for plugins, allowing messages to be logged with an associated plugin name and optional debug logging control.
/// </summary>
class PluginLogger : public ILogger
{
private:
	std::shared_ptr<ILogger> m_baseLogger;  // Shared base logger for actual output
	std::string m_pluginName;
	bool m_debugEnabled = false;

public:
	PluginLogger(std::shared_ptr<ILogger> baseLogger, std::string pluginName);

	void log(LogLevel level, const std::string& message) override;

	void enableDebugLogging() override;

	void disableDebugLogging() override;

	bool isDebugLoggingEnabled() const override;

	void setPluginName(const std::string& name) override;
};
