#include <format>
#include <memory>
#include <ranges>

#include "Plugin/PluginInstance.h"
#include "Plugin/PluginRuntimeContext.h"
#include "Plugin/PluginManager.h"

#include <cassert>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Utils/range_utils.h"


static constexpr std::string_view configurationLocation = "config/plugins_config.json";

namespace
{
	std::expected<PluginManagerConfig, std::string> loadPluginConfig(const std::filesystem::path& configPath)
	{
		PluginManagerConfig config;
		std::ifstream file(configPath);
		if(!file)
		{
			return std::unexpected(std::format("Could not file PluginManger config file at '{}'", configPath.string()));
		}

		nlohmann::json j;
		file >> j;

		if (j.contains("autoScan")) config.autoScan = j["autoScan"].get<bool>();
		if (j.contains("pluginDirectory")) config.pluginDirectory = j["pluginDirectory"].get<std::string>();
		if (j.contains("pluginScanInterval_Seconds")) config.pluginScanInterval = std::chrono::seconds(j["pluginScanInterval"].get<int>());
		if (j.contains("enabledPlugins")) config.enabledPluginsOnStartup = j["enabledPlugins"].get<std::vector<std::string>>();

		return config;
	}
}

PluginManager::PluginManager(std::shared_ptr<ILogger>& logger) : m_engineLogger(logger)
{
	assert(logger && "logger was nullptr");
}

void PluginManager::init()
{
	if(const auto config = loadPluginConfig(configurationLocation))
	{
		scanPluginsFolder(config.value().pluginDirectory);

		logMessage(LogLevel::Info, std::format("Loading previously enabled Plugin [{}]", join_range(config.value().enabledPluginsOnStartup)));
		for(const std::string& pluginName : config.value().enabledPluginsOnStartup)
		{
			auto knownPlugin = m_discoveredPlugins.find(pluginName);
			if(knownPlugin != m_discoveredPlugins.end())
			{
				loadPlugin(knownPlugin->second.path, knownPlugin->second.name);
			}
		}

		return;
	}

	logMessage(LogLevel::Error, std::format("Could not find PluginManager configuration file",configurationLocation));
}

bool PluginManager::isPluginFolderWatcherEnabled() const
{
	return m_scanningThread.joinable();
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

void PluginManager::removeKnownPlugin(const std::string& name)
{
	logMessage(LogLevel::Info, std::format("'{}' removed from known plugins", name));
	m_discoveredPlugins.erase(name);
}

void PluginManager::scanPluginsFolder(const std::string& pluginDirectory)
{
	logMessage(LogLevel::Info, "Scanning plugin folder");
	if(!std::filesystem::exists(pluginDirectory))
	{
		m_engineLogger->log(LogLevel::Info, std::format("Could not find plugin at '{}'", pluginDirectory));
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(pluginDirectory))
	{
		if (!entry.is_regular_file()) continue;
		if (entry.path().extension() != PLUGIN_EXT) continue;

		//Add this plugin to the list
		const std::string discoveredPluginName = entry.path().stem().string();

		const auto it = m_discoveredPlugins.find(discoveredPluginName);
		if(it == m_discoveredPlugins.end())
		{
			logMessage(LogLevel::Info, std::format("Found new Plugin '{}'", discoveredPluginName));
			m_discoveredPlugins.emplace(entry.path().stem().string(),
				PluginInfo
				{
					.name = discoveredPluginName,
					.path = entry.path(),
					.loaded = false,
					.errorMessage = ""
				}
			);
		}
	}
}

const std::unordered_map<std::string, PluginInfo> PluginManager::getDiscoveredPlugins() const
{
	return m_discoveredPlugins;
}

bool PluginManager::loadPlugin(const std::filesystem::path& path, const std::string& name)
{
	if (!std::filesystem::exists(path))
	{
		logMessage(LogLevel::Error, std::format("Plugin path '{}' does not exist.", path.string()));
		removeKnownPlugin(name.empty() ? path.stem().string() : name);
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

			removeKnownPlugin(pluginName);
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

void PluginManager::startPluginAutoScan()
{
	logMessage(LogLevel::Info, std::format("Starting plugin folder watcher: scanning '{}' every {}s", m_config.pluginDirectory, m_config.pluginScanInterval.count()));
	if(!m_config.autoScan || m_scanningThread.joinable())
	{
		return;
	}

	m_scanningThread = std::jthread([this](std::stop_token stop_token) {
		while (!stop_token.stop_requested()) {
			scanPluginsFolder(m_config.pluginDirectory);

			std::this_thread::sleep_for(m_config.pluginScanInterval);
		}
	});
}

void PluginManager::stopPluginAutoScan()
{
	logMessage(LogLevel::Info, "Stopping plugin folder watcher.");
	m_scanningThread.request_stop();
}
