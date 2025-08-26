#include "NumberLogger_Plugin.h"
#include "Services/IServiceSpecialisations.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"

NumberLoggerHandler::NumberLoggerHandler(ServiceProxy<ILogger> logger): m_logger(std::move(logger))
{

}

bool NumberLoggerHandler::handleType(const std::shared_ptr<int>& data)
{
	m_logger->log(LogLevel::Debug, std::format("Detected number: {}", *data));
	return false;
}


std::expected<void, std::string> NumberLoggerPlugin::onPluginLoad(IPluginContext& context)
{
	m_logger = ServiceProxy(context.getService<ILogger>());

	//m_handler = std::make_shared<NumberLoggerHandler>(m_logger);

	context.registerTypedHandler<int>(m_handler);

	m_logger->log (LogLevel::Info, "NumberLoggerPlugin registered for int packets");

	return {};
}



void NumberLoggerPlugin::onPluginUnload()
{
}

std::type_index NumberLoggerPlugin::getType() const
{
	return typeid(int);
}

EXPORT PluginDescriptor* getPluginDescriptor()
{
	static PluginDescriptor descriptor
	{
		.name = "NumberLogger",
		.version = "1.0.0",
		.services = {
			{
				.type = typeid(ILogger),
				.name = "logger",
				.minVersion = ">=1.0.0",
				.required = false
			}
	}
	};

	return &descriptor;
}

EXPORT IPlugin* loadPlugin()
{
	return new NumberLoggerPlugin();
}
