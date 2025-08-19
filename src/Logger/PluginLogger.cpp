#include "Services/Logging/PluginLogger.h"

#include "Services/Logging/SpdLogger.h"
#include "Services/Logging/UILogSink.h"



PluginLogger::PluginLogger(std::shared_ptr<ILogger> baseLogger, std::string pluginName) : m_underlyingLogger(baseLogger), m_pluginName(pluginName)
{
	m_underlyingLogger->setPluginName(m_pluginName);
}

void PluginLogger::log(LogLevel level, const std::string& message)
{
	m_underlyingLogger->log(level, message);
}

void PluginLogger::enableDebugLogging()
{
	m_underlyingLogger->enableDebugLogging();
}

void PluginLogger::disableDebugLogging()
{
	m_underlyingLogger->disableDebugLogging();
}

bool PluginLogger::isDebugLoggingEnabled() const
{
	return m_underlyingLogger->isDebugLoggingEnabled();
}

void PluginLogger::setPluginName(const std::string& name)
{
	m_pluginName = name;
}
