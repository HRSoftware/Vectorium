#pragma once

#include <string>
#include "LogLevel.h"

class ILogger
{
public:
	virtual ~ILogger() = default;
	virtual void log(LogLevel level, const std::string& message) = 0;

	void enabledDebugLogging();
	void disableDebugLogging();

	bool isDebugLoggingEnabled() const;

	protected:
		bool m_isDebugLogEnabled = false;
};

inline void ILogger::enabledDebugLogging()
{
	m_isDebugLogEnabled = true;
}

inline void ILogger::disableDebugLogging()
{
	m_isDebugLogEnabled = false;
}

inline bool ILogger::isDebugLoggingEnabled() const
{
	return m_isDebugLogEnabled;
}
