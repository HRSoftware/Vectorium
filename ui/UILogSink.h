#pragma once

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/spdlog.h"

#include <mutex>
#include <deque>

struct UILogEntry
{
	std::string message;
	spdlog::level::level_enum level;
};

class UILogSink : public spdlog::sinks::base_sink<std::mutex>
{
	public:

	std::deque<UILogEntry> logBuffer;
	size_t maxEntries = 200;

protected:
	void sink_it_(const spdlog::details::log_msg& msg) override;

	void flush_() override;
};