#pragma once

#include <spdlog/spdlog.h>
#include "Logger/ILogger.h"

class SpdLogger : public ILogger
{
public:
	void log(LogLevel level, const std::string& msg) override;
};