#include "Services/Logging/ILogger.h"


bool ILogger::isDebugLoggingEnabled() const
{
	return m_isDebugLogEnabled;
}

void ILogger::setPluginName(const std::string& name)
{
	m_pluginName = name;
}
