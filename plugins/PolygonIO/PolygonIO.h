#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#include "Plugin/IPlugin.h"

struct PolygonIO_Candle
{
	std::chrono::system_clock::time_point timestamp;
	double open;
	double high;
	double low;
	double close;
	uint64_t volume;
};

class PolygonIO_Plugin : public IPlugin
{
	public:
		void onPluginLoad(IPluginContext& context) override;
		void onPluginUnload() override;
		void onRender() override;
		std::type_index getType() const override;

		const std::vector<PolygonIO_Candle>& getCandles() const;

	private:
		void queryLatestCandles();

		std::vector<PolygonIO_Candle> m_candleHistory;
		std::mutex m_dataMutex;

		std::jthread m_pollThread;
		std::atomic_bool m_running = false;

		std::string m_APIKey = "EQYZuQWtWMRKXg9_kmjdSLU6pEy2FVzv";
		std::string m_symbol = "AAPL";
		std::chrono::seconds m_pollInterval = std::chrono::seconds(20);

		ServiceProxy<ILogger>     m_logger{nullptr};
		ServiceProxy<IRestClient> m_RESTClient{nullptr};
		std::string m_pluginName = "PolygonIO";
};