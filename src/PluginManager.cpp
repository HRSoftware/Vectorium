#include "../include/Plugin/PluginManager.h"
#include "../include/Plugin/LoadedPlugin.h"
#include "../include/Plugin/PluginContextImpl.h"

#include <ranges>
#include <memory>
#include <format>

PluginManager::PluginManager(std::shared_ptr<ILogger>& logger) : engineLogger(logger)
{
}

PluginInfo& PluginManager::getOrAddPluginInfo(const std::filesystem::path& path)
{
	const std::string name = path.stem().string();

	// Look for existing entry
	for (auto& plugin : discoveredPlugins)
	{
		if (plugin.name == name)
		{
			return plugin;
		}
	}

	// Otherwise create a new one
	discoveredPlugins.push_back(PluginInfo{
		.path = path,
		.name = name,
		.loaded = false,
		.errorMessage = ""
		});
	return discoveredPlugins.back();
}

void PluginManager::scanPluginsFolder()
{
	const std::filesystem::path pluginPath = "plugins_bin";

	if(!std::filesystem::exists(pluginPath))
	{
		engineLogger->log(LogLevel::Info, std::format("Could not find plugin at '{}'", pluginPath.string()));
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginPath))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != PLUGIN_EXT) continue;

		//Add this plugin to the list
		discoveredPlugins.push_back({
			.path = entry.path(),
			.name = entry.path().stem().string(),
		});
	}
}

std::span<const PluginInfo> PluginManager::getDiscoveredPlugins() const
{
	return discoveredPlugins;
}

std::unordered_map<std::string, std::unique_ptr<LoadedPlugin>>& PluginManager::getLoadedPlugins()
{
	return loadedPlugins;
}

bool PluginManager::loadPlugin(const std::filesystem::path& path, const std::string& name)
{
	if(!std::filesystem::exists(path))
	{
		engineLogger->log(LogLevel::Error, std::format("Could not find plugin at '{}'", path.string()));
		return false;
	}

	std::string pluginName = name;
	if(pluginName.empty())
	{
		pluginName = path.stem().string();
	}

	auto& info = getOrAddPluginInfo(path);
	if(info.loaded)
	{
		//Already loaded;
		return true;
	}

	try
	{
		const LibraryHandle handle = LoadSharedLibrary(path.string().c_str());
		if (!handle) {
			logMessage(LogLevel::Error, std::format("Failed to load plugin '{}' - ({})\n", pluginName, getError()));
			return false;
		}

		const auto create = reinterpret_cast<IPlugin * (*)()> (GetSymbol(handle, "initPlugin"));
		if (!create) {
			UnloadLibrary(handle);
			logMessage(LogLevel::Info, std::format("Could not find 'initPlugin' entry point in plugin '{}' - ({})\n", pluginName, getError()));
			return false;
		}

		std::unique_ptr<IPlugin>           plugin(create());
		auto context = std::make_unique<PluginContextImpl>(engineLogger, pluginName);
		plugin->registerType(*context);

		// Load the plugin
		auto newPlugin = std::make_unique<LoadedPlugin>(handle, std::move(plugin), std::move(context), path.stem().string());

		logMessage(LogLevel::Info, std::format("[PluginManager] - Loaded plugin '{}'\n", pluginName));

		info.loaded = true;
		info.errorMessage.clear();
		loadedPlugins.emplace(pluginName, std::move(newPlugin));

		return true;
	}
	catch(const std::exception& ex)
	{
		info.errorMessage = ex.what();
		return false;
	}
}

bool PluginManager::unloadPlugin(const std::string& name)
{
	auto plugin = loadedPlugins.find(name);
	if(plugin != loadedPlugins.end())
	{
		return plugin->second->unload();
	}

	logMessage(LogLevel::Error, std::format("[PluginManager] - Error unloading plugin '{}'\n", name));
	return false;
}

std::vector<std::string> PluginManager::getNamesOfAllLoadedPlugins() const
{
	std::vector<std::string> keys;
	keys.reserve(loadedPlugins.size());

	for(const auto& key : loadedPlugins | std::views::keys )
	{
		keys.push_back(key);
	}

	return keys;
}

void PluginManager::logMessage(LogLevel logLvl, const std::string& msg) const
{
	if(engineLogger)
	{
		engineLogger->log(logLvl, msg);
	}
}
