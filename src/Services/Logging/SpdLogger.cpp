
#include <Services/Logging/SpdLogger.h>
#include <string>
#include "spdlog/spdlog.h"

SpdLogger::SpdLogger(const std::string& name, const std::vector<spdlog::sink_ptr>& sinks)
{
	m_pluginName = name;
	m_logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());

	m_logger->set_level(spdlog::level::debug);
	m_logger->info("SpdLogger constructor test - immediate log");
}

void SpdLogger::log(LogLevel level, const std::string& msg)
{
	switch (level)
	{
		case LogLevel::Debug:
			if (isDebugLoggingEnabled())
			{
				m_logger->debug(msg);
			} break;
		case LogLevel::Info:    m_logger->info(msg); break;
		case LogLevel::Warning: m_logger->warn(msg); break;
		case LogLevel::Error:   m_logger->error(msg); break;
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
