#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

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
		std::expected<void, std::string> onPluginLoad(IPluginContext& context) override;
		void onPluginUnload() override;
		void onRender() override;
		std::type_index getType() const override;

		const std::vector<PolygonIO_Candle>& getCandles() const;

	private:
		void queryLatestCandles();

	public:
		void tick() override;
		~PolygonIO_Plugin() override;

	private:
		void runAPIThread(std::stop_token);
		void testConnection();

	private:
		std::vector<PolygonIO_Candle> m_candleHistory;
		std::mutex                    m_dataMutex;

		std::jthread m_apiThread;
		std::atomic_bool m_running = false;
		std::condition_variable_any m_sleepCondition;

		std::string m_APIKey = "EQYZuQWtWMRKXg9_kmjdSLU6pEy2FVzv";
		std::string m_symbol = "AAPL";
		std::chrono::seconds m_pollInterval = std::chrono::seconds(10);

		ServiceProxy<ILogger>     m_logger{nullptr};
		ServiceProxy<IRestClient> m_RESTClient{nullptr};
		std::string               m_pluginName = "PolygonIO";
		std::string               m_baseURL = "https://api.polygon.io";
};