#include "format"
#include "../include/Plugin/PluginInstance.h"
#include "../include/Plugin/IPlugin.h"
#include "../include/Plugin/PluginRuntimeContext.h"
#include <utility>
#include <format>



PluginInstance::PluginInstance(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginRuntimeContext> ctx, std::string name)
: m_handle(h)
, m_plugin(std::move(p))
, m_context(std::move(ctx))
, m_pluginName(
	std::move(name))
, m_logger(m_context->getLoggerShared())
{
}

PluginInstance::~PluginInstance()
{
	unload();
};

PluginInstance::PluginInstance(PluginInstance&& other) noexcept
: m_handle(std::exchange(other.m_handle, nullptr))
, m_plugin(std::move(other.m_plugin))
, m_context(std::move(other.m_context))
{

}

PluginInstance& PluginInstance::operator=(PluginInstance&& other) noexcept
{
	if (this != &other)
	{
		unload();
		this->m_handle = std::exchange(other.m_handle, nullptr);
		this->m_plugin = std::move(other.m_plugin);
		this->m_context = std::move(other.m_context);
	}

	return *this;
}

IPlugin* PluginInstance::get() const
{
	return m_plugin.get();
}

PluginRuntimeContext* PluginInstance::getContext() const
{
	return m_context.get();
}

void PluginInstance::tick() const
{
	if(m_plugin)
	{
		m_plugin->onPluginTick();
	}
}

bool PluginInstance::unload()
{
	if(m_plugin)
	{
		if(m_context)
		{
			m_context->unregisterDataPacketHandler();
		}

		m_plugin.reset();

		log(LogLevel::Info, "unloaded plugin");
	}

	// If we have a handle on the plugin, then close it
	if(m_handle)
	{
		UnloadLibrary(m_handle);
	}

	return true;
}

void PluginInstance::log(LogLevel level, const std::string& msg) const
{
	if(m_logger)
	{
		m_logger->log(level, msg);
	}
}

std::expected<std::type_index, std::string> PluginInstance::getType() const
{
	if(m_plugin)
	{
		return m_plugin->getType();
	}

	return std::unexpected("Could not get type for plugin");
}
