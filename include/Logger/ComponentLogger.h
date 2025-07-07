#pragma once
#include "Logger/ILogger.h"
#include <memory>
#include <string>

class ComponentLogger : public ILogger
{
public:
	ComponentLogger(std::shared_ptr<ILogger> baseLogger, std::string prefix);

	void log(LogLevel level, const std::string& message) override;

private:
	std::shared_ptr<ILogger> base;
	std::string prefix;
};