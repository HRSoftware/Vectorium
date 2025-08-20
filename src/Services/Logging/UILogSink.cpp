#include "Services/Logging/UILogSink.h"


void UILogSink::clearBuffer()
{
	logBuffer.clear();
}

void UILogSink::sink_it_(const spdlog::details::log_msg& msg)
{
	// Format the message using the current formatter
	spdlog::memory_buf_t formatted;
	base_sink<std::mutex>::formatter_->format(msg, formatted);

	// Remove oldest entries if we're at capacity
	if (logBuffer.size() >= maxEntries)
	{
		logBuffer.pop_front();
	}

	// Add new entry
	logBuffer.emplace_back(UILogEntry{
		.message = fmt::to_string(formatted),
		.level = msg.level,
		.timestamp = std::chrono::system_clock::from_time_t(std::chrono::duration_cast<std::chrono::seconds>(msg.time.time_since_epoch()).count())
		});
}

void UILogSink::flush_()
{
	logBuffer.clear();
}
