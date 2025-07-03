#include <iostream>
#include <string>
#include <vector>
#include <format>
#include "../include/Core/PluginLoading.h"
#include "../include/Core/PluginContextImpl.h"
#include "../include/Core/IPlugin.h"


void tryLoadPlugin(const std::string& path, std::vector<LoadedPlugin>& vec) noexcept
{
	const LibraryHandle handle = LoadSharedLibrary(path.c_str());
	if(!handle)
	{
		std::cout << std::format("Failed to load plugin '{}' - ({})", path, getError() << "\n";
		return;
	}

	const auto create = reinterpret_cast<IPlugin * (*)()> (GetSymbol(handle, "initPlugin"));
	if(!create)
	{
		UnloadLibrary(handle);
		std::cout << std::format("Could not find 'initPlugin' entry point in plugin '{}' - ({})", path, getError() << "\n";
		return;
	}

	std::unique_ptr<IPlugin> plugin(create());
	auto                     context = std::make_unique<PluginContextImpl>();
	plugin->registerType(*context);
	vec.emplace_back(handle, std::move(plugin), std::move(context));
}
