#pragma once

#include <string>
#include "LogLevel.h"

class ILogger
{
public:
	virtual ~ILogger() = default;
	virtual void log(LogLevel level, const std::string& message) = 0;
};