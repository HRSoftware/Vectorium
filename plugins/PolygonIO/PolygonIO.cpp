#include "PolygonIO.h"

#include <utility>
#include "Services/IServiceSpecialisations.h"

std::expected<void, std::string> PolygonIO_Plugin::onPluginLoad(IPluginContext& context)
{
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

	if (context.hasService<IRestClient>()) 
	{
		m_RESTClient = ServiceProxy(context.getService<IRestClient>());

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
		return std::unexpected("Context does not have the service 'IRestClient'");
	}

	m_running = true;

	m_apiThread = std::jthread([this](const std::stop_token& token)
	{
		runAPIThread(token);
	});
}

void PolygonIO_Plugin::onPluginUnload()
{
	m_running = false;
	if(m_apiThread.joinable())
	{
		m_apiThread.request_stop();
		m_sleepCondition.notify_all();
	}
}

void PolygonIO_Plugin::onRender()
{
	IPlugin::onRender();
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

PolygonIO_Plugin::~PolygonIO_Plugin()
{
	m_running = false;

	if(m_apiThread.joinable())
	{
		m_apiThread.request_stop();
		m_sleepCondition.notify_all();
	}
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

			std::unique_lock lock(m_dataMutex);
			m_sleepCondition.wait_for(
				lock,
				token,
				std::chrono::seconds(m_pollInterval),
				[&token]
			{
				return token.stop_requested();
			}
			);
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
				.type = typeid(IRestClient),
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