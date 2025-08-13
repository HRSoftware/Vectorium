#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "PluginManagerConfig.h"
#include "Plugin/PluginInstance.h"


enum class LogLevel;
class PluginInstance;
//class ILogger;
class DataPacketRegistry;

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
		bool loadConfig();
		bool saveConfig() const;
		PluginManager(std::shared_ptr<ILogger> logger);
		PluginManager(std::shared_ptr<ILogger> logger, std::shared_ptr<DataPacketRegistry> ptrDataPacketReg);

		PluginInfo&                                               getOrAddPluginInfo(const std::string& pluginName, const std::filesystem::path& path = "");
		void                                                      removeKnownPlugin(const std::string& name);
		void                                                      scanPluginsFolder(const std::string& pluginDirectory);
		[[nodiscard]] std::unordered_map<std::string, PluginInfo> getDiscoveredPlugins() const;

		bool                        loadPlugin(const std::filesystem::path& path, const std::string& = "");
		bool                        unloadPlugin(const std::string& name);

		[[nodiscard]] std::vector<std::string>                                                getNamesOfAllLoadedPlugins() const;
		[[nodiscard]] const std::unordered_map<std::string, std::unique_ptr<PluginInstance>>& getLoadedPlugins() const;
		void init();

		bool isPluginFolderWatcherEnabled() const;

		void startPluginAutoScan();
		void stopPluginAutoScan();


		// Config related -- Redundant?
		PluginManagerConfig&               getConfig();
		void                               setScanInterval(std::chrono::seconds scanInterval);
		[[nodiscard]] std::chrono::seconds getScanInterval() const;

		void                                setPluginFolderDir(const std::string& dirPath);
		[[nodiscard]] std::filesystem::path getPluginFolderDir() const;

		void               setAutoScan(bool bShouldScan);
		[[nodiscard]] bool getAutoScanEnabled() const;
		void               reloadPluginConfig();

		void enableDebugLogging();
		void disableDebugLogging();

		bool isDebugLoggingEnabled();

		void tick();

	//void reloadPlugin(const std::string name);

private:
	std::unordered_map<std::string, PluginInfo> m_discoveredPlugins;
	std::unordered_map<std::string, std::unique_ptr<PluginInstance>> m_loadedPlugins;
	std::shared_ptr<DataPacketRegistry> m_dataPacketRegistry;
	std::shared_ptr<ILogger> m_engineLogger;
	void logMessage(LogLevel logLvl, const std::string& msg) const;

	std::string m_configurationLocation = "config/plugins_config.json";

	PluginManagerConfig m_config;

	std::jthread m_scanningThread;
};
