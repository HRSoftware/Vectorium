#include "PolygonIO.h"
#include "Services/IServiceSpecialisations.h"

void PolygonIO_Plugin::onPluginLoad(IPluginContext& context)
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

	if(context.hasService<IRestClient>())
	{
		m_RESTClient = ServiceProxy(context.getService<IRestClient>());
		m_RESTClient->set_bearer_token(m_APIKey);
		m_RESTClient->setBaseUrl(m_baseURL);
		m_logger->log(LogLevel::Info, "REST service added");
	}
	else
	{
		context.log(LogLevel::Error, "Context does not have the service 'IRestClient'");
		return;
	}

	m_running = true;

	

	m_apiThread = std::jthread([this](std::stop_token token)
	{
		runAPIThread(token);
	});

}

void PolygonIO_Plugin::onPluginUnload()
{
	if(m_apiThread.joinable())
	{
		m_apiThread.request_stop();
		m_sleepCondition.notify_all();
	}
	m_running = false;
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
	m_logger->log(LogLevel::Debug, "Querying lastest candles SENT");

	auto statusResponse = m_RESTClient->GET("/v3/reference/tickers?market=stocks&active=true&order=asc&limit=100&sort=ticker");

	m_logger->log(LogLevel::Debug, "Querying lastest candles RESPONSE");
	if(!statusResponse.has_value())
	{
		m_logger->log(LogLevel::Error, std::format("GET request returned - {}", statusResponse.error().error));
		return;
	}

	m_logger->log(LogLevel::Info, RESTUtils::formatForLogging(statusResponse.value()));
}

void PolygonIO_Plugin::tick()
{
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