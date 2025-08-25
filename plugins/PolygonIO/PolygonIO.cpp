#include "PolygonIO.h"

#include <utility>
#include "Services/IServiceSpecialisations.h"

PolygonIO_Plugin::~PolygonIO_Plugin()
{
	m_running = false;

	if(m_apiThread.joinable())
	{
		m_apiThread.request_stop();
		m_sleepCondition.notify_all();
	}
}

std::expected<void, std::string> PolygonIO_Plugin::onPluginLoad(IPluginContext& context)
{

	// Debug: Check context received
	std::cout << "PolygonIO onPluginLoad - Context address: " << (void*)&context << std::endl;
	std::cout << "Plugin name from context: " << context.getPluginName() << std::endl;


	if(context.hasService<ILogger>())
	{
		m_logger = ServiceProxy(context.getService<ILogger>());
		m_logger->setPluginName(m_pluginName);
		m_logger->log(LogLevel::Info, "Logger service added");
	}
	else
	{
		context.log(LogLevel::Error, "Context does not have the service 'ILogger'");
	}

	if (context.hasService<IPluginRESTService>())
	{
		m_RESTClient = ServiceProxy(context.getService<IPluginRESTService>());
		m_logger->log(LogLevel::Info, "REST service added");

		m_RESTClient->set_bearer_token(m_APIKey);
		m_RESTClient->setBaseUrl(m_baseURL);

		m_RESTClient->set_timeout(std::chrono::milliseconds(15000)); // 15 second timeout

		HeaderMap defaultHeaders;
		defaultHeaders["User-Agent"] = "Vectorium/1.0";
		defaultHeaders["Accept"] = "application/json";
		defaultHeaders["Host"] = "api.polygon.io";
		defaultHeaders["Authorization"] = std::format("Bearer {}", m_APIKey );

		m_RESTClient->set_default_headers(defaultHeaders);

		m_logger->log(LogLevel::Info, "REST service added");
	}
	else
	{
		return std::unexpected("Context does not have the service 'IPluginRESTService'");
	}

	if(context.hasService<IPluginUIService>())
	{
		m_uiService = ServiceProxy(context.getService<IPluginUIService>());
		m_logger->log(LogLevel::Info, "UI service added");

		if(m_uiService.isAvailable())
		{
			//Register the main UI callback with the service
			m_uiService->registerPluginUIRenderer(context.getPluginName(), [this]()
			{
				renderPluginUI();
			});

			m_logger->log(LogLevel::Info, "UI service registered");
		}
	}
	else
	{
		m_logger->log(LogLevel::Info, "No UI service available - running headless");
	}


	m_running = true;
	m_showMainWindow = true;

	m_apiThread = std::jthread([this](const std::stop_token& token)
	{
		runAPIThread(token);
	});

	return {};
}

void PolygonIO_Plugin::onPluginUnload()
{
	m_logger->log(LogLevel::Info, "PolygonIO Plugin unloading");

	if (m_uiService.isAvailable())
	{
		m_uiService->unregisterPluginUIRenderer(m_pluginName);
	}

	m_running = false;
	if(m_apiThread.joinable())
	{
		m_apiThread.request_stop();
		m_sleepCondition.notify_all();
	}
}

void PolygonIO_Plugin::renderPluginUI()
{
	static int call_count = 0;
	call_count++;

	// Check if ImGui context is valid
	ImGui::SetCurrentContext(m_uiService->getImGuiContext());
	if (!ImGui::GetCurrentContext())
	{
		m_logger->log(LogLevel::Warning, "No ImGui context available for plugin rendering");
		return;
	}

	// Check if we're in a valid frame
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
	{
		// Might not be in a valid render state
		m_logger->log(LogLevel::Debug, "ImGui not in active render state");
		return;
	}

	m_logger->log(LogLevel::Debug, std::format("PolygonIO::renderPluginUI() call #{}", call_count));

	if (m_showMainWindow)
	{
		try
		{
			renderMainWindow();
		}
		catch (const std::exception& e)
		{
			m_logger->log(LogLevel::Error, std::format("Error in renderMainWindow: {}", e.what()));
		}
	}
}

std::type_index PolygonIO_Plugin::getType() const
{
	return typeid(PolygonIO_Candle);
}

const std::vector<PolygonIO_Candle>& PolygonIO_Plugin::getCandles() const
{
	return m_candleHistory;
}

void PolygonIO_Plugin::queryLatestCandles()
{
	m_logger->log(LogLevel::Debug, "Querying latest candles SENT");

	//HeaderMap headers;
	//headers["Host"] = "fakerapi.it";

	auto statusResponse = m_RESTClient->GET("/v1/marketstatus/now");

	m_logger->log(LogLevel::Debug, "Querying latest candles RESPONSE");

	if(!statusResponse.has_value())
	{
		// Log more detailed error information
		m_logger->log(LogLevel::Error, std::format("GET request failed: {}", statusResponse.error().error));
		return;
	}

	m_logger->log(LogLevel::Info, std::format("Response status: {}", statusResponse.value().status));
	m_logger->log(LogLevel::Info, RESTUtils::formatForLogging(statusResponse.value()));

}

void PolygonIO_Plugin::tick()
{
	m_logger->log(LogLevel::Debug, "Tick");
}

void PolygonIO_Plugin::runAPIThread(std::stop_token token)
{
	m_logger->log(LogLevel::Debug, "API Thread started");
	while (!token.stop_requested() && m_running)
	{
		try
		{
			queryLatestCandles();
			//testConnection();
			auto timeout = std::chrono::seconds(1);  // Check every second instead of full interval
			auto remaining = m_pollInterval;

			while(remaining > std::chrono::seconds(0) && !token.stop_requested() && m_running)
			{
				auto sleepTime = std::min(timeout, remaining);

				std::unique_lock lock(m_dataMutex);
				auto result = m_sleepCondition.wait_for(lock, sleepTime);
				lock.unlock();  // Release lock immediately

				if (token.stop_requested() || !m_running) {
					break;
				}

				remaining -= sleepTime;
			}
		}
		catch (const std::exception& ex)
		{
			m_logger->log(LogLevel::Error, ex.what());

			std::unique_lock lock(m_dataMutex);
			m_sleepCondition.wait_for(
				lock,
				token,
				std::chrono::seconds(5),
				[&token]
				{
					return token.stop_requested();
				}
			);
		}
	}

	//m_logger->log(LogLevel::Debug, "API Thread exited");
}

void PolygonIO_Plugin::testConnection()
{
	//m_RESTClient->testConnection();
	// Simple test: set everything fresh each time
	m_logger->log(LogLevel::Debug, "=== BEARER TOKEN TEST ===");

	// Set base URL
	m_RESTClient->setBaseUrl("https://fakerapi.it");

	// Set bearer token  
	//m_RESTClient->set_bearer_token("EQYZuQWtWMRKXg9_kmjdSLU6pEy2FVzv");

	
	auto statusResponse = m_RESTClient->GET(R"(/api/v2/addresses)");

	if(!statusResponse.has_value())
	{
		// Check if it's an HTTPS issue
		if (statusResponse.error().error.find("SSL") != std::string::npos || 
			statusResponse.error().error.find("TLS") != std::string::npos) {
			m_logger->log(LogLevel::Error, "This appears to be an SSL/HTTPS issue. Check if SSL is enabled in cmake.");
		}
		return;
	}

	m_logger->log(LogLevel::Info, std::format("Response status: {}", statusResponse.value().status));
	m_logger->log(LogLevel::Info, "Response body (should show Authorization header):");
	m_logger->log(LogLevel::Info, RESTUtils::formatForLogging(statusResponse.value()));
}

void PolygonIO_Plugin::renderMainWindow()
{
	if (ImGui::Begin("PolygonIO Market Data", &m_showMainWindow))
	{
		ImGui::Text("Hello from PolygonIO Plugin!");


		ImGui::Text("Connection Status: {}", m_uiState.connectionStatus);

		ImGui::Text("Requests Made: {}", m_uiState.requestCount);

		if (ImGui::Button("Test Button"))
		{
			m_logger->log(LogLevel::Info, "Test button clicked!");
		}

		/*if (m_pluginContext->uiButton("Apply Configuration")) {
			applyConfiguration();
		}*/
	}
	ImGui::End();
}

bool PolygonIO_Plugin::hasUIWindow() const
{
	return true;
}

std::string PolygonIO_Plugin::getUIWindowTitle() const
{
	return m_pluginName;
}

bool PolygonIO_Plugin::isUIWindowVisible() const
{
	return m_showMainWindow;
}

void PolygonIO_Plugin::setUIWindowVisible(bool visible)
{
	m_showMainWindow = visible;
	m_logger->log(LogLevel::Info, std::format("PolygonIO window visibility set to: {}", visible));
}

void PolygonIO_Plugin::toggleUIWindow()
{
	m_showMainWindow = !isUIWindowVisible();
}

EXPORT PluginDescriptor* getPluginDescriptor()
{
	static PluginDescriptor descriptor
	{
		.name = "PolygonIO",
		.version = "1.0.0",
		.services = {
			{
				.type = typeid(ILogger),
				.name = "logger",
				.minVersion = ">=1.0.0",
				.required = false
			},
			{
				.type = typeid(IPluginRESTService),
				.name = "RESTClient",
				.minVersion = ">=1.0.0",
				.required = true
			}
	}
	};

	return &descriptor;
}

EXPORT IPlugin* loadPlugin()
{
	return new PolygonIO_Plugin();
}