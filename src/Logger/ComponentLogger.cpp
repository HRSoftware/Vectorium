#include "Logger/ComponentLogger.h"

#include <cassert>

ComponentLogger::ComponentLogger(std::shared_ptr<ILogger> baseLogger, std::string prefix)
	: m_baseLogger(std::move(baseLogger)), m_loggingPrefix(std::move(prefix))
{
	assert(m_baseLogger && "baseLogger is nullptr");
}

void ComponentLogger::log(LogLevel level, const std::string& message)
{
	assert(m_baseLogger && "baseLogger is nullptr");
	if(m_baseLogger)
	{
		m_baseLogger->log(level, "[" + m_loggingPrefix + "] " + message);
	}
}
