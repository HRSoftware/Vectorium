#include "Logger/LoggablePlugin.h"

void LoggablePlugin::setLogger(std::shared_ptr<ILogger> logger, std::string name)
{
	m_logger = std::move(logger);
	m_componentName = std::move(name);
}

void LoggablePlugin::unsetLogger()
{
	m_logger = nullptr;
}

void LoggablePlugin::log(LogLevel level, const std::string& msg) const
{
	if (m_logger)
	{
		m_logger->log(level, msg);
	}
	else
	{
		// Optional fallback (for debug/test use)
#ifndef NDEBUG
		std::cerr << "[WARN] Logger not initialized: " << msg << "\n";
#endif
	}
}
