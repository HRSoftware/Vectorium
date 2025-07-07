#pragma once
#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "LoadedPlugin.h"

enum class LogLevel;
class ILogger;

struct PluginInfo
{
	std::filesystem::path path;
	std::string name;
	bool loaded = false;
	std::string errorMessage; // If failed to load
};

/// <summary>
/// PluginManager is used to track and handle the lifetime of all loaded plugins
/// </summary>
class PluginManager
{
public:
	explicit PluginManager(std::shared_ptr<ILogger>& logger);

	PluginInfo&                 getOrAddPluginInfo(const std::filesystem::path& path);
	void                        scanPluginsFolder();
	std::span<const PluginInfo> getDiscoveredPlugins() const;

	std::unordered_map<std::string, std::unique_ptr<LoadedPlugin>>& getLoadedPlugins();

	bool                        loadPlugin(const std::filesystem::path& path, const std::string& = "");
	bool                        unloadPlugin(const std::string& name);

	[[nodiscard]] std::vector<std::string> getNamesOfAllLoadedPlugins() const;

	//void reloadPlugin(const std::string name);

private:
	std::vector<PluginInfo> discoveredPlugins;
	std::unordered_map<std::string, std::unique_ptr<LoadedPlugin>> loadedPlugins;
	std::shared_ptr<ILogger> engineLogger;
	void logMessage(LogLevel logLvl, const std::string& msg) const;
};
