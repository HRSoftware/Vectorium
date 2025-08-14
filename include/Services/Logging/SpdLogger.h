#pragma once

#include <memory>

#include "Services/Logging/ILogger.h"

namespace spdlog
{
	class logger;
}


class SpdLogger : public ILogger
{
public:
	SpdLogger(std::string logName = "Default");
	void log(LogLevel level, const std::string& msg) override;
	void enableDebugLogging() override;
	void disableDebugLogging() override;
	bool isDebugLoggingEnabled() const override;
	void setPluginName(const std::string& name) override;

	private:
	bool m_isDebugLogEnabled{false};
	std::string  m_pluginName;
	std::shared_ptr<spdlog::logger> m_logger;
};
