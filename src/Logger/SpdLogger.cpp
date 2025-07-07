#include "SpdLogger.h"

void SpdLogger::log(LogLevel level, const std::string& msg)
{
	switch (level)
	{
		case LogLevel::Debug:   spdlog::debug(msg); break;
		case LogLevel::Info:    spdlog::info(msg); break;
		case LogLevel::Warning: spdlog::warn(msg); break;
		case LogLevel::Error:   spdlog::error(msg); break;
	}
}