

#include "Services/Logging/SpdLogger.h"

#include <string>

#include "spdlog/spdlog.h"

void SpdLogger::log(LogLevel level, const std::string& msg)
{
	switch (level)
	{
		case LogLevel::Debug:
			if (isDebugLoggingEnabled())
			{
				spdlog::debug(msg);
			}; break;
		case LogLevel::Info:    spdlog::info(msg); break;
		case LogLevel::Warning: spdlog::warn(msg); break;
		case LogLevel::Error:   spdlog::error(msg); break;
	}
}

void SpdLogger::enableDebugLogging()
{
	m_isDebugLogEnabled = true;
	spdlog::set_level(spdlog::level::debug);
	log(LogLevel::Debug, "Debug logging enabled");
}

void SpdLogger::disableDebugLogging()
{
	log(LogLevel::Debug, "Debug logging disabled");
	m_isDebugLogEnabled = false;
	spdlog::set_level(spdlog::level::info);
}
