#include <format>
#include <memory>
#include <ranges>

#include "Plugin/PluginInstance.h"
#include "Plugin/PluginRuntimeContext.h"
#include "Plugin/PluginManager.h"

#include <assert.h>

PluginManager::PluginManager(std::shared_ptr<ILogger>& logger) : m_engineLogger(logger)
{
	assert(logger && "logger was nullptr");
}

PluginInfo& PluginManager::getOrAddPluginInfo(const std::string& pluginName, const std::filesystem::path& pluginPath)
{
	std::string name = pluginPath.stem().string();

	if(!pluginName.empty())
	{
		name = pluginName;
	}

	// Insert if not exists; get reference either way
	auto [iter, inserted] = m_discoveredPlugins.try_emplace(name, PluginInfo{
		.name = name,
		.path = pluginPath,
		.loaded = false,
		.errorMessage = ""
		});

	return iter->second;
}

void PluginManager::scanPluginsFolder()
{
	const std::filesystem::path pluginPath = "plugins";

	if(!std::filesystem::exists(pluginPath))
	{
		m_engineLogger->log(LogLevel::Info, std::format("Could not find plugin at '{}'", pluginPath.string()));
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginPath))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != PLUGIN_EXT) continue;

		//Add this plugin to the list
		m_discoveredPlugins.emplace(entry.path().stem().string(),
			PluginInfo
			{
				.name = entry.path().stem().string(),
				.path = entry.path(),
				.loaded = false,
				.errorMessage = ""
			}
		);
	}
}

const std::unordered_map<std::string, PluginInfo>& PluginManager::getDiscoveredPlugins() const
{
	return m_discoveredPlugins;
}

bool PluginManager::loadPlugin(const std::filesystem::path& path, const std::string& name)
{
	if (!std::filesystem::exists(path))
	{
		logMessage(LogLevel::Error, std::format("Plugin path '{}' does not exist.", path.string()));
		return false;
	}

	std::string pluginName = name.empty() ? path.stem().string() : name;

	// Already loaded
	if (m_loadedPlugins.contains(pluginName))
	{
		logMessage(LogLevel::Warning, std::format("Plugin '{}' is already loaded.", pluginName));
		return true;
	}

	// Get or create plugin metadata
	PluginInfo& info = getOrAddPluginInfo(pluginName, path);

	try
	{
		// Load shared library
		LibraryHandle handle = LoadSharedLibrary(path.string().c_str());
		if (!handle)
		{
			info.errorMessage = getError();
			logMessage(LogLevel::Error, std::format("Failed to load plugin '{}': {}", pluginName, info.errorMessage));
			return false;
		}

		// Load init symbol
		auto pluginEntryFunction = reinterpret_cast<IPlugin*(*)()>(GetSymbol(handle, "initPlugin"));
		if (!pluginEntryFunction)
		{
			info.errorMessage = getError();
			UnloadLibrary(handle);
			logMessage(LogLevel::Error, std::format("Missing 'initPlugin' in '{}': {}", pluginName, info.errorMessage));
			return false;
		}

		// Construct plugin and context(currently just one type of Context)
		auto assignedPluginContext = std::make_unique<PluginRuntimeContext>(m_engineLogger, pluginName);

		std::unique_ptr<IPlugin> plugin(pluginEntryFunction());
		plugin->onPluginLoad(*assignedPluginContext);

		// Package plugin instance
		auto pluginInstance = std::make_unique<PluginInstance>(
			handle,
			std::move(plugin),
			std::move(assignedPluginContext),
			pluginName
		);

		info.loaded = true;
		info.errorMessage.clear();

		m_loadedPlugins.emplace(pluginName, std::move(pluginInstance));

		logMessage(LogLevel::Info, std::format("Successfully loaded plugin '{}'", pluginName));
		return true;
	}
	catch (const std::exception& ex)
	{
		info.errorMessage = ex.what();
		logMessage(LogLevel::Error, std::format("Exception while loading plugin '{}': {}", pluginName, ex.what()));
		return false;
	}
}

bool PluginManager::unloadPlugin(const std::string& name)
{
	const auto it = m_loadedPlugins.find(name);
	if(it != m_loadedPlugins.end())
	{
		// Unload and remove from collection of loaded plugins
		const auto bSuccess = it->second->unload();
		m_loadedPlugins.erase(it);

		// Update the collection of all known plugins
		auto discovered = m_discoveredPlugins.find(name);
		if(discovered != m_discoveredPlugins.end())
		{
			discovered->second.loaded = false;
		}

		return bSuccess;
	}

	logMessage(LogLevel::Error, std::format("Plugin '{}' is not loaded; cannot unload.", name));
	return false;
}

std::vector<std::string> PluginManager::getNamesOfAllLoadedPlugins() const
{
	std::vector<std::string> keys;
	keys.reserve(m_loadedPlugins.size());

	for(const auto& key : m_loadedPlugins | std::views::keys )
	{
		keys.push_back(key);
	}

	return keys;
}

const std::unordered_map<std::string, std::unique_ptr<PluginInstance>>& PluginManager::getLoadedPlugins() const
{
	return m_loadedPlugins;
}

void PluginManager::logMessage(const LogLevel logLvl, const std::string& msg) const
{
	if(m_engineLogger)
	{
		m_engineLogger->log(logLvl, std::format("[{}] - {}", "PluginManager", msg));
	}
}
