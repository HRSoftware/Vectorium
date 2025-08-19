#include "Plugin/PluginInstance.h"
#include <utility>
#include "Plugin/IPlugin.h"
#include "Plugin/PluginRuntimeContext.h"

#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"

PluginInstance::PluginInstance(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginRuntimeContext> ctx, std::string name)
: m_handle(h)
, m_plugin(std::move(p))
, m_context(std::move(ctx))
, m_pluginName(std::move(name))
{
	if (!m_handle)
	{
		throw std::invalid_argument("Nullptr handle passed to PluginInstance constructor");
	}

	if (!m_plugin)
	{
		throw std::invalid_argument("Nullptr plugin passed to PluginInstance constructor");
	}

	if(!m_context)
	{
		throw std::invalid_argument("Nullptr context passed to PluginInstance constructor");
	}
}

PluginInstance::~PluginInstance()
{
	if (m_plugin)
	{
		m_plugin->onPluginUnload();
		m_plugin.reset();
	}

	if(m_context)
	{
		m_context.reset();
	}

	// If we have a handle on the plugin, then close it
	if(m_handle)
	{
		UnloadLibrary(m_handle);
	}
};

IPlugin* PluginInstance::getPlugin() const
{
	if(!m_plugin)
	{
		return nullptr;
	}

	return m_plugin.get();
}

PluginRuntimeContext* PluginInstance::getContext() const
{
	if(!m_context)
	{
		return nullptr;
	}
	return m_context.get();
}

const std::string& PluginInstance::getPluginName()
{
	return m_pluginName;
}

void PluginInstance::enablePluginDebugLogging()
{
	m_context->getLoggerShared()->enableDebugLogging();
}

void PluginInstance::disablePluginDebugLogging()
{
	m_context->getLoggerShared()->disableDebugLogging();
}

bool PluginInstance::isPluginDebugLoggingEnabled() const
{
	return m_context->getLoggerShared()->isDebugLoggingEnabled();
}

void PluginInstance::tick() const
{
	if(m_plugin)
	{
		m_plugin->tick();
	}
}

