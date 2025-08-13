#include "Plugin/IPluginContext.h"

#include "Services/Logging/ILogger.h"

 void IPluginContext::enableDebugLogging()
{
	if (const auto logger = getLoggerShared())
	{
		logger->enableDebugLogging();
	}
}

void IPluginContext::disableDebugLogging()
{
	if (const auto logger = getLoggerShared())
	{
		logger->disableDebugLogging();
	}
}

bool IPluginContext::isDebugLoggingEnabled() const
{
	const auto logger = const_cast<IPluginContext*>(this)->getLoggerShared();
	return logger ? logger->isDebugLoggingEnabled() : false;
}
