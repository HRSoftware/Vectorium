#include "PolygonIO.h"
#include "Services/IServiceSpecialisations.h"

void PolygonIO_Plugin::onPluginLoad(IPluginContext& context)
{
	if(context.hasService<ILogger>())
	{
		context.log(LogLevel::Error, "Context does not have the service 'ILogger'");
	}
	else
	{
		m_logger = context.getService<ILogger>();
		m_logger->setPluginName(m_pluginName);
	}

	if(context.hasService<IRestClient>())
	{
		context.log(LogLevel::Error, "Context does not have the service 'IRestClient'");
	}
	else
	{
		m_RESTClient = context.getService<IRestClient>();
	}

	m_running = true;

	m_pollThread = std::jthread([this](std::stop_token st)
	{
		while (!st.stop_requested() && m_running)
		{
			queryLatestCandles();
			std::this_thread::sleep_for(m_pollInterval);
		}
	});

}

void PolygonIO_Plugin::onPluginUnload()
{
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
}

EXPORT IPlugin* loadPlugin()
{
	return new PolygonIO_Plugin();
}