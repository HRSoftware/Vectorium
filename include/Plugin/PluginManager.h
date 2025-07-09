#pragma once
#include <expected>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "PluginInstance.h"

enum class LogLevel;
class ILogger;

struct PluginInfo
{
	std::string name;
	std::filesystem::path path;
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

	PluginInfo&                               getOrAddPluginInfo(const std::string& pluginName, const std::filesystem::path& path = "");
	void                                      scanPluginsFolder();
	const std::unordered_map<std::string, PluginInfo>& getDiscoveredPlugins() const;

	

	bool                        loadPlugin(const std::filesystem::path& path, const std::string& = "");
	bool                        unloadPlugin(const std::string& name);

	[[nodiscard]] std::vector<std::string> getNamesOfAllLoadedPlugins() const;
	[[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<PluginInstance>>& getLoadedPlugins() const;

	//void reloadPlugin(const std::string name);

private:
	std::unordered_map<std::string, PluginInfo> m_discoveredPlugins;
	std::unordered_map<std::string, std::unique_ptr<PluginInstance>> m_loadedPlugins;
	std::shared_ptr<ILogger> m_engineLogger;
	void logMessage(LogLevel logLvl, const std::string& msg) const;
};
