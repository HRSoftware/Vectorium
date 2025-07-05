#include "../include/Plugin/PluginManager.h"
#include "../include/Plugin/LoadedPlugin.h"
#include <ranges>
#include <memory>

#include "../include/Plugin/PluginContextImpl.h"

void PluginManager::loadPluginsFolder()
{
	const std::filesystem::path pluginPath = "plugins_bin";

	if(!std::filesystem::exists(pluginPath))
	{
		std::cout << std::format("Could not find plugin at '{}'", pluginPath.string());
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginPath))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != PLUGIN_EXT) continue;

		loadPlugin(entry.path(), entry.path().filename().string());
	}
}

bool PluginManager::loadPlugin(const std::filesystem::path& path, const std::string& name)
{
	if(!std::filesystem::exists(path))
	{
		std::cout << std::format("Could not find plugin at '{}'", path.string());
		return false;
	}

	const LibraryHandle handle = LoadSharedLibrary(path.string().c_str());
	if (!handle)
	{
		std::cout << std::format("Failed to load plugin '{}' - ({})\n", path.string(), getError());
		return false;
	}

	const auto create = reinterpret_cast<IPlugin * (*)()> (GetSymbol(handle, "initPlugin"));
	if (!create) 
	{
		UnloadLibrary(handle);
		std::cout << std::format("Could not find 'initPlugin' entry point in plugin '{}' - ({})\n", path.string(), getError());
		return false;
	}

	std::unique_ptr<IPlugin>           plugin(create());
	auto context = std::make_unique<PluginContextImpl>();
	plugin->registerType(*context);

	auto newPlugin = std::make_unique<LoadedPlugin>(handle, std::move(plugin), std::move(context), path.filename().string());

	std::cout << std::format("[PluginManager] - Loaded plugin '{}'\n", path.filename().string());

	loadedPlugins.push_back(std::move(newPlugin));
	return true;
}

std::vector<std::string> PluginManager::getLoadedPluginNames() const
{
	std::vector<std::string> keys;
	keys.reserve(loadedPlugins.size());

	for(const auto& key : loadedPlugins )
	{
		if(key.get()->getType().has_value())
		{
			keys.emplace_back(key.get()->getType().value().name());
		}
	}

	return keys;
}