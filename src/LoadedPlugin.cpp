#include "format"
#include "../include/Plugin/LoadedPlugin.h"
#include "../include/Plugin/IPlugin.h"
#include "../include/Plugin/PluginContextImpl.h"
#include <utility>
#include <format>



LoadedPlugin::LoadedPlugin(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginContextImpl> ctx, std::string name)
: handle(h)
, plugin(std::move(p))
, context(std::move(ctx))
, pluginName(
	std::move(name))
{}

LoadedPlugin::~LoadedPlugin()
{
	unload();
};

LoadedPlugin::LoadedPlugin(LoadedPlugin&& other) noexcept
: handle(std::exchange(other.handle, nullptr))
, plugin(std::move(other.plugin))
, context(std::move(other.context))
{

}

LoadedPlugin& LoadedPlugin::operator=(LoadedPlugin&& other) noexcept
{
	if (this != &other)
	{
		unload();
		this->handle = std::exchange(other.handle, nullptr);
		this->plugin = std::move(other.plugin);
		this->context = std::move(other.context);
	}

	return *this;
}

IPlugin* LoadedPlugin::get() const
{
	return plugin.get();
}

PluginContextImpl* LoadedPlugin::getContext() const
{
	return context.get();
}

bool LoadedPlugin::unload()
{
	//std::cout << std::format("[Plugin] - unloaded plugin '%'\n", pluginName);
	
	if(plugin)
	{
		if(context)
		{
			context->unregisterDataPacketHandler(plugin->getType());
		}

		plugin.reset();
	}

	// If we have a handle on the plugin, then close it
	if(handle)
	{
		UnloadLibrary(handle);
	}

	return true;
}

std::expected<std::type_index, std::string> LoadedPlugin::getType() const
{
	if(plugin)
	{
		return plugin->getType();
	}

	return std::unexpected("Could not get type for plugin");
}