#pragma once

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "Plugin/IPlugin.h"
#include "UI/PluginUIHelpers.h"

struct PolygonIO_Candle
{
	std::chrono::system_clock::time_point timestamp;
	double open;
	double high;
	double low;
	double close;
	uint64_t volume;
};

struct PolygonIO_MarketStatus
{
	std::string market;
	std::string serverTime;
	std::string exchanges;
	bool afterHours = false;
	bool earlyHours = false;
};

struct PolygonIO_UIState
{
	char apiKeyBuffer[256] = "EQYZuQWtWMRKXg9_kmjdSLU6pEy2FVzv";
	char symbolBuffer[16] = "AAPL";
	int pollIntervalSeconds = 10;
	bool autoUpdate = true;

	// Display settings
	bool showRawData = false;
	bool showChart = true;
	int maxCandles = 100;

	// Status
	std::string lastError;
	std::string connectionStatus = "Disconnected";
	int requestCount = 0;
	std::chrono::system_clock::time_point lastRequestTime;

	// Market status
	PolygonIO_MarketStatus marketStatus;
	bool hasMarketStatus = false;
};

class PolygonIO_Plugin : public IPlugin
{
	public:
		~PolygonIO_Plugin() override;

		std::expected<void, std::string> onPluginLoad(IPluginContext& context) override;
		void                             onPluginUnload() override;
		void                             renderPluginUI();
		std::type_index                  getType() const override;

		const std::vector<PolygonIO_Candle>& getCandles() const;
		void tick() override;

	private:
		void queryLatestCandles();

		void runAPIThread(std::stop_token);
		void testConnection();
		void renderMainWindow();

	private:
		void updateUIData();
		ImU32 getCandleColor(const PolygonIO_Candle& candle) const;
		std::string formatTimestamp(const std::chrono::system_clock::time_point& time) const;
		std::string formatCurrency(double value) const;

		//UI Related
	public:
		bool        hasUIWindow() const override;
		std::string getUIWindowTitle() const override;
		bool        isUIWindowVisible() const override;
		void        setUIWindowVisible(bool visible) override;
		void        toggleUIWindow() override;

	private:

		std::vector<PolygonIO_Candle> m_candleHistory;
		std::mutex                    m_dataMutex;

		std::jthread m_apiThread;
		std::atomic_bool m_running = false;
		std::condition_variable_any m_sleepCondition;

		std::string m_APIKey = "EQYZuQWtWMRKXg9_kmjdSLU6pEy2FVzv";
		std::string m_symbol = "AAPL";
		std::chrono::seconds m_pollInterval = std::chrono::seconds(10);

		ServiceProxy<ILogger>     m_logger{ nullptr };
		ServiceProxy<IPluginRESTService> m_RESTClient{ nullptr };
		ServiceProxy<IPluginUIService>  m_uiService{ nullptr };

		std::string               m_pluginName = "PolygonIO";
		std::string               m_baseURL = "https://api.polygon.io";

		PolygonIO_UIState m_uiState;
		bool m_showMainWindow = true;
};