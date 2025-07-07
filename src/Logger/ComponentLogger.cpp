#include "Logger/ComponentLogger.h"

ComponentLogger::ComponentLogger(std::shared_ptr<ILogger> baseLogger, std::string prefix)
	: base(std::move(baseLogger)), prefix(std::move(prefix))
{}

void ComponentLogger::log(LogLevel level, const std::string& message)
{
	base->log(level, "[" + prefix + "] " + message);
}
