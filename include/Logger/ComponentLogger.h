#pragma once
#include <memory>
#include <string>

#include "Logger/ILogger.h"

class ComponentLogger : public ILogger
{
public:
	ComponentLogger(std::shared_ptr<ILogger> baseLogger, std::string prefix);

	void log(LogLevel level, const std::string& message) override;

private:
	std::shared_ptr<ILogger> m_baseLogger;
	std::string m_loggingPrefix;
};