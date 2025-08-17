#pragma once

#include <memory>

#include "Services/Logging/ILogger.h"
#include "spdlog/common.h"

namespace spdlog
{
	class logger;
}

/// <summary>
/// Implements a logger using spdlog, providing methods for logging messages, managing debug logging, and setting a plugin name.
/// </summary>
class SpdLogger : public ILogger
{
public:
	SpdLogger(const std::string& name, const std::vector<spdlog::sink_ptr>& sinks);
	void         log(LogLevel level, const std::string& msg) override;
	void         enableDebugLogging() override;
	void         disableDebugLogging() override;
	bool         isDebugLoggingEnabled() const override;
	void         setPluginName(const std::string& name) override;

	private:
	bool m_isDebugLogEnabled{false};
	std::string  m_pluginName;
	std::shared_ptr<spdlog::logger> m_logger;
};
