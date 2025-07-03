#include "format"
#include "../include/Core/PluginLoading.h"
#include "../include/Core/IPlugin.h"
#include "../include/Core/PluginContextImpl.h"
#include <utility>



LoadedPlugin::LoadedPlugin(const std::string& path)
{
}

LoadedPlugin::LoadedPlugin(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginContextImpl> ctx) : handle(h), plugin(std::move(p)), context(std::move(ctx))
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

void LoadedPlugin::unload()
{
	if(plugin)
	{
		if(context)
		{
			context->unregisterHandler(plugin->getType());
		}

		plugin.reset();
	}

	// If we have a handle on the plugin, then close it
	if(handle)
	{
		UnloadLibrary(handle);
	}
}

std::expected<std::type_index, std::string> LoadedPlugin::getType() const
{
	if(plugin)
	{
		return plugin->getType();
	}

	return std::unexpected("Could not get type for plugin");
}

