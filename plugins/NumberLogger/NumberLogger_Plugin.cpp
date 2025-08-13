#include "NumberLogger_Plugin.h"
#include "Services/IServiceSpecialisations.h"

NumberLoggerHandler::NumberLoggerHandler(ServiceProxy<ILogger> logger): m_logger(std::move(logger))
{

}

bool NumberLoggerHandler::handleType(const std::shared_ptr<int>& data)
{
	m_logger->log(LogLevel::Debug, std::format("Detected number: {}", *data));
	return false;
}


void NumberLoggerPlugin::onPluginLoad(IPluginContext& context)
{
	m_logger = context.getService<ILogger>();

	//m_handler = std::make_shared<NumberLoggerHandler>(m_logger);

	context.registerTypedHandler<int>(m_handler);

	m_logger->log (LogLevel::Info, "NumberLoggerPlugin registered for int packets");
}



void NumberLoggerPlugin::onPluginUnload()
{
}

std::type_index NumberLoggerPlugin::getType() const
{
	return typeid(int);
}

EXPORT IPlugin* loadPlugin()
{
	return new NumberLoggerPlugin();
}
