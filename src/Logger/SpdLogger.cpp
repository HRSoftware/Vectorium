

#include <string>
#include <Services/Logging/SpdLogger.h>
#include "spdlog/spdlog.h"

SpdLogger::SpdLogger(const std::string& name, const std::vector<spdlog::sink_ptr>& sinks)
{
	m_pluginName = name;
	m_logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
}

void SpdLogger::log(LogLevel level, const std::string& msg)
{
	switch (level)
	{
		case LogLevel::Debug:
			if (isDebugLoggingEnabled())
			{
				spdlog::debug(msg);
			} break;
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

bool SpdLogger::isDebugLoggingEnabled() const
{
	return m_isDebugLogEnabled;
}

void SpdLogger::setPluginName(const std::string& name)
{
	m_pluginName = name;
}
