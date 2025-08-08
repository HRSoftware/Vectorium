#include "UILogSink.h"

void UILogSink::sink_it_(const spdlog::details::log_msg& msg)
{
	spdlog::memory_buf_t formatted;
	spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);

	if (logBuffer.size() >= maxEntries)
	{
		logBuffer.pop_front();
	}

	logBuffer.emplace_back(UILogEntry{
		.message = fmt::to_string(formatted),
		.level = msg.level
	});
}

void UILogSink::flush_()
{
	logBuffer.clear();
}
