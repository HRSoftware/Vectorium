#include "Services/Logging/PluginLogger.h"

#include "Services/Logging/SpdLogger.h"
#include "Services/Logging/UILogSink.h"



PluginLogger::PluginLogger(std::shared_ptr<ILogger> baseLogger, std::string pluginName) : m_baseLogger(baseLogger), m_pluginName(pluginName)
{
}

void PluginLogger::log(LogLevel level, const std::string& message)
{

	if (level == LogLevel::Debug && !m_debugEnabled) {
		return;  // Don't log debug if disabled for this plugin
	}

	m_baseLogger->log(level, "[" + m_pluginName + "] " + message);
}

void PluginLogger::enableDebugLogging()
{ 
	m_debugEnabled = true;
}

void PluginLogger::disableDebugLogging()
{ 
	m_debugEnabled = false;
}

bool PluginLogger::isDebugLoggingEnabled() const
{ 
	return m_debugEnabled;
}

void PluginLogger::setPluginName(const std::string& name)
{
	m_pluginName = name;
}
