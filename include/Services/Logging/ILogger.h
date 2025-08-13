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

	bool isDebugLoggingEnabled() const;
	void setPluginName(const std::string& name);

	protected:
		bool m_isDebugLogEnabled = false;
		std::string m_pluginName;
};

inline bool ILogger::isDebugLoggingEnabled() const
{
	return m_isDebugLogEnabled;
}

inline void ILogger::setPluginName(const std::string& name)
{
	m_pluginName = name;
}
