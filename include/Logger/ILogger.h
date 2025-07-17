#pragma once

#include <string>
#include "LogLevel.h"
#include "spdlog/spdlog.h"

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
	spdlog::set_level(spdlog::level::debug);
}

inline void ILogger::disableDebugLogging()
{
	m_isDebugLogEnabled = false;
	spdlog::set_level(spdlog::level::info);
}

inline bool ILogger::isDebugLoggingEnabled() const
{
	return m_isDebugLogEnabled;
}
