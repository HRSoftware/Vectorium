#pragma once
#include "Logger/ILogger.h"

enum class LogLevel;

class MockLogger : public ILogger
{
public:
	void log(LogLevel level, const std::string& message) override;
};
