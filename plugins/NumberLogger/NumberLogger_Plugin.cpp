#include "NumberLogger_Plugin.h"

bool NumberLoggerHandler::handleType(const std::shared_ptr<int>& data)
{
	log(LogLevel::Info, std::format("Detected number: {}", *data));
	return false;
}

void NumberLoggerPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLoggerShared(), "NumberLogger");

	m_handler = std::make_shared<NumberLoggerHandler>();
	m_handler->setLogger(context.getLoggerShared(), "NumberLoggerHandler");

	context.registerTypedHandler<int>(m_handler);

	log(LogLevel::Info, "NumberLoggerPlugin registered for int packets");
}

void NumberLoggerPlugin::onPluginUnload()
{
	
}

std::type_index NumberLoggerPlugin::getType() const
{
	return typeid(int);
}

EXPORT IPlugin* initPlugin()
{
	return new NumberLoggerPlugin();
}
