#pragma once
#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "PluginInstance.h"

enum class LogLevel;
class ILogger;

struct PluginManagerConfig
{
	bool autoScan = true;
	std::string pluginDirectory = "plugins";
	std::chrono::seconds pluginScanInterval = std::chrono::seconds(5);
	std::vector<std::string> enabledPluginsOnStartup;
};

struct PluginInfo
{
	std::string name;
	std::filesystem::path path;
	bool loaded = false;
	std::string errorMessage; // If it failed to load
};

/// <summary>
/// PluginManager is used to track and handle the lifetime of all loaded plugins
/// </summary>
class PluginManager
{
public:
	explicit PluginManager(std::shared_ptr<ILogger>& logger);

	PluginInfo&                                                      getOrAddPluginInfo(const std::string& pluginName, const std::filesystem::path& path = "");
	void removeKnownPlugin(const std::string& name);
	void                                                             scanPluginsFolder(const std::string& pluginDirectory);
	[[nodiscard]] const std::unordered_map<std::string, PluginInfo> getDiscoveredPlugins() const;

	bool                        loadPlugin(const std::filesystem::path& path, const std::string& = "");
	bool                        unloadPlugin(const std::string& name);

	[[nodiscard]] std::vector<std::string>                                                getNamesOfAllLoadedPlugins() const;
	[[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<PluginInstance>>& getLoadedPlugins() const;
	void init();

	bool isPluginFolderWatcherEnabled() const;

	void startPluginAutoScan();
	void stopPluginAutoScan();

	//void reloadPlugin(const std::string name);

private:
	std::unordered_map<std::string, PluginInfo> m_discoveredPlugins;
	std::unordered_map<std::string, std::unique_ptr<PluginInstance>> m_loadedPlugins;
	std::shared_ptr<ILogger> m_engineLogger;
	void logMessage(LogLevel logLvl, const std::string& msg) const;

	PluginManagerConfig m_config;

	std::jthread m_scanningThread;
};
