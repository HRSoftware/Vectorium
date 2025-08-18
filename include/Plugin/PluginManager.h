#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "PluginManagerConfig.h"
#include "Plugin/PluginInstance.h"

class IRestClient;
struct PluginDescriptor;
class IPluginContext;
enum class LogLevel;
class PluginInstance;
class DataPacketRegistry;

struct PluginInfo
{
	std::string name;
	std::filesystem::path path;
	bool loaded = false;
	std::string errorMessage; // If it failed to load
};

/// <summary>
/// Manages the lifecycle, configuration, and discovery of plugins within the application.
/// </summary>
class PluginManager
{
public:
	bool loadConfig();
	bool saveConfig() const;
	PluginManager(ILogger& logger, DataPacketRegistry& ptrDataPacketReg, std::shared_ptr<IRestClient> RESTClient);
	std::unique_ptr<IPluginContext> createContextForPlugin(const PluginDescriptor* desc, const std::string& pluginName);


	/// <summary>
	/// Retrieves the PluginInfo for a plugin by name, or adds a new entry if it does not exist.
	/// </summary>
	/// <param name="pluginName">The name of the plugin. If empty, the name is derived from the pluginPath.</param>
	/// <param name="pluginPath">The filesystem path to the plugin.</param>
	/// <returns>A reference to the PluginInfo associated with the specified plugin name.</returns>
	PluginInfo&                                               getOrAddPluginInfo(const std::string& pluginName, const std::filesystem::path& path = "");
	void                                                      removeKnownPlugin(const std::string& name);
	void                                                      scanPluginsFolder(const std::string& pluginDirectory);

	/// <summary>
	/// Returns a map of discovered plugins.
	/// </summary>
	/// <returns>An unordered map where each key is a plugin name (string) and each value is a PluginInfo object representing the discovered plugin.</returns>
	[[nodiscard]] std::unordered_map<std::string, PluginInfo> getDiscoveredPlugins() const;


	bool loadPlugin(const std::filesystem::path& path, const std::string& = "");
	bool unloadPlugin(const std::string& name);

	/// <summary>
	/// Retrieves the names of all currently loaded plugins.
	/// </summary>
	/// <returns>A vector<string> containing the names of all loaded plugins.</returns>
	[[nodiscard]] std::vector<std::string> getNamesOfAllLoadedPlugins() const;

	/// <summary>
	/// Returns a reference to the collection of loaded plugins.
	/// </summary>
	/// <returns>A constant reference to an unordered map associating plugin names (as strings) with unique pointers to PluginInstance objects.</returns>
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

	/// <summary>
	/// Reloads the plugin configuration by loading the configuration data again.
	/// </summary>
	void               reloadPluginConfig();

	void enablePluginDebugLogging(const std::string& pluginName);
	void disablePluginDebugLogging(const std::string& pluginName);
	bool isPluginDebugLoggingEnabled(const std::string& pluginName) const;

	void tick();

	//void reloadPlugin(const std::string name);

	private:
	/// <summary>
	/// Registers appropriate services for a plugin based on its descriptor and runtime context.
	/// </summary>
	/// <param name="context">The runtime context for the plugin, used to register services.</param>
	/// <param name="desc">The descriptor containing metadata about the plugin, such as its name.</param>
	void registerServicesForPlugin(PluginRuntimeContext* context, const PluginDescriptor* desc) const;
	void logMessage(LogLevel logLvl, const std::string& msg) const;
	std::shared_ptr<ILogger> createPluginLogger(const std::string& pluginName) const;

	private:
		std::filesystem::path getExecutableDir() const;
		std::filesystem::path getPortableConfigPath() const;

private:
	std::unordered_map<std::string, PluginInfo> m_discoveredPlugins;
	std::unordered_map<std::string, std::unique_ptr<PluginInstance>> m_loadedPlugins;

	DataPacketRegistry& m_dataPacketRegistry;

	//Services from Engine
	ILogger& m_baseLogger;
	std::shared_ptr<IRestClient> m_restClient;
	std::string m_configurationFileName = "plugins_config.json";

	PluginManagerConfig m_config;

    std::jthread m_scanningThread;
};
