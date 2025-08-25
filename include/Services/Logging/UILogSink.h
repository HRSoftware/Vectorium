#pragma once

#include <chrono>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/common.h>
#include <mutex>
#include <deque>
#include <string>

struct UILogEntry
{
	std::string message;
	spdlog::level::level_enum level;
	std::chrono::system_clock::time_point timestamp;
};

class UILogSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
	std::deque<UILogEntry> logBuffer;
	size_t maxEntries = 200;

	void clearBuffer();

protected:
	void sink_it_(const spdlog::details::log_msg& msg) override;
	void flush_() override;
};