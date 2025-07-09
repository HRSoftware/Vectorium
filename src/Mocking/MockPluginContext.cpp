#include "Mocking/MockPluginContext.h"

#include "Mocking/MockLogger.h"

MockPluginContext::MockPluginContext(std::string name): m_pluginName(std::move(name)),
                                                        m_logger(std::make_shared<MockLogger>())
{
}

std::shared_ptr<ILogger> MockPluginContext::getLoggerShared()
{
	return m_logger;
}

const std::unordered_map<std::type_index, std::vector<std::shared_ptr<IDataPacketHandler>>>& MockPluginContext::getRegisteredHandlers() const
{
	return m_handlers;
}

std::string MockPluginContext::getPluginName() const
{
	return m_pluginName;
}

