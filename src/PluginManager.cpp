#include "Plugin/PluginManager.h"

#include <cassert>
#include <expected>
#include <format>
#include <fstream>
#include <memory>
#include <ranges>
#include <utility>
#include <nlohmann/json.hpp>

#include "DataPacket/DataPacketRegistry.h"
#include "Plugin/PluginInstance.h"
#include "Plugin/PluginRuntimeContext.h"
#include "Services/ServiceId.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include "Services/Logging/PluginLogger.h"
#include "Services/Logging/SpdLogger.h"
#include "Services/REST/RestClient_HttpLib.h"
#include "spdlog/common.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "Utils/range_utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <linux/limits.h>
#endif

namespace
{
	std::expected<std::string, std::string> formatPluginDescriptor(PluginDescriptor* pPluginDescriptor)
	{
		if (!pPluginDescriptor)
		{
			return std::unexpected("Error: Service Descriptor was nullptr");
		}

		std::string stringOfServices = std::format("'{}(v{})' requires:\n", pPluginDescriptor->name, pPluginDescriptor->version);

		for (auto& service : pPluginDescriptor->services)
		{
			stringOfServices += std::format("{}({}) - optional: {}\n", service.name, service.minVersion, service.required ? "false" : "true"); // negate as we are showing optional rather than required
		}

		return stringOfServices;
	}

	bool needsNetworkAccess(const PluginDescriptor* desc)
	{
		// Check if plugin requests IRestClient in its services list
		return std::ranges::any_of(desc->services,
			[](const auto& service)
			{
				return service.type == typeid(IRestClient);
			});
	}

	bool isTrustedPlugin(const std::string& name)
	{
		static const std::set<std::string> trusted = {"GPS", "WeatherService"};
		return trusted.contains(name);
	}

	std::filesystem::path getExecutableDir()
	{
#ifdef _WIN32
		char path[MAX_PATH];
		GetModuleFileNameA(NULL, path, MAX_PATH);
		return std::filesystem::path(path).parent_path();
#else
		char path[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
		if (count == -1) return ".";
		std::string exePath(path, count);
		return std::filesystem::path(exePath).parent_path();
#endif
	}
}


PluginManager::PluginManager(ILogger& logger,
	DataPacketRegistry& ptrDataPacketReg
	, std::shared_ptr<IRestClient> RESTClient
	, spdlog::sink_ptr uiLogSink)
	: m_uiLogSink(std::move(uiLogSink))
	, m_dataPacketRegistry(ptrDataPacketReg)
	, m_baseLogger(logger)
	, m_restClient(std::move(RESTClient))
{
}

bool PluginManager::loadConfig()
{
	const auto configPath = getPortableConfigPath().string();
	log(LogLevel::Info, std::format("Load Config path: - {}", configPath));
	std::ifstream file(configPath);
	if(!file)
	{
		log(LogLevel::Error, std::format("Could not read plugin config '{}'", configPath));
		if(saveConfig())
		{
			reloadPluginConfig();
			return true;
		}
		return false;
	}

	nlohmann::json j;
	file >> j;

	if (j.contains("autoScan")) m_config.autoScan = j["autoScan"].get<bool>();
	if (j.contains("pluginDirectory")) m_config.pluginDirectory = j["pluginDirectory"].get<std::string>();
	if (j.contains("pluginScanInterval_seconds")) m_config.pluginScanInterval = std::chrono::seconds(j["pluginScanInterval_seconds"].get<int>());
	if (j.contains("enabledPlugins")) m_config.enabledPluginsOnStartup = j["enabledPlugins"].get<std::vector<std::string>>();

	return true;
}

bool PluginManager::saveConfig() const
{
	const auto configPath = getPortableConfigPath().string();
	log(LogLevel::Info, std::format("Save Config path: - {}", configPath));
	if(!std::filesystem::exists(configPath))
	{
		log(LogLevel::Warning, std::format("Creating default plugin_config file at '{}'", configPath));
	}

	std::ofstream file(configPath);
	if(!file)
	{
		log(LogLevel::Error, std::format("Could not create new PluginManger config file at '{}'", configPath));
		return false;
	}

	nlohmann::json json =
	{
		{"autoScan", m_config.autoScan},
		{"pluginDirectory", m_config.pluginDirectory},
		{"pluginScanInterval_seconds", m_config.pluginScanInterval.count()}
		// add enabled plugins
	};

	file << std::setw(4) << json << "\n";
	return true;
}


/// <summary>
/// Initializes the PluginManager by loading configuration, scanning for plugins, loading enabled plugins, and starting auto-scan if configured.
/// </summary>
void PluginManager::init()
{
	if(loadConfig())
	{
		const auto pluginFolderLoc = getExecutableDir() / m_config.pluginDirectory;
		m_baseLogger.log(LogLevel::Info, std::format("Checking plugin folder: {}", pluginFolderLoc.string()));
		scanPluginsFolder(std::format("{}", pluginFolderLoc.string()));

		log(LogLevel::Info, std::format("Loading previously enabled Plugin [{}]", join_range(m_config.enabledPluginsOnStartup)));

		for(const std::string& pluginName : m_config.enabledPluginsOnStartup)
		{
			auto knownPlugin = m_discoveredPlugins.find(pluginName);
			if(knownPlugin != m_discoveredPlugins.end())
			{
				loadPlugin(knownPlugin->second.path, knownPlugin->second.name);
			}
		}

		if(m_config.autoScan)
		{
			startPluginAutoScan();
		}


		return;
	}

	log(LogLevel::Error, std::format("Could not find PluginManager configuration file '{}'", m_configurationFileName));
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
	auto [iter, inserted] = m_discoveredPlugins.try_emplace(name, PluginInfo
		{
			.name = name,
			.path = pluginPath,
			.loaded = false,
			.errorMessage = ""
		});

	return iter->second;
}

void PluginManager::removeKnownPlugin(const std::string& name)
{
	log(LogLevel::Info, std::format("'{}' removed from known plugins", name));
	m_discoveredPlugins.erase(name);
}

void PluginManager::scanPluginsFolder(const std::string& pluginDirectory)
{
	log(LogLevel::Debug, "Scanning plugin folder");
	if(!std::filesystem::exists(pluginDirectory))
	{
		m_baseLogger.log(LogLevel::Info, std::format("Could not find 'plugins' folder at '{}'", pluginDirectory));
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
			log(LogLevel::Info, std::format("Found plugin '{}'", discoveredPluginName));
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


std::unordered_map<std::string, PluginInfo> PluginManager::getDiscoveredPlugins() const
{
	return m_discoveredPlugins;
}

bool PluginManager::loadPlugin(const std::filesystem::path& path, const std::string& name)
{
	std::string pluginName = name.empty() ? path.stem().string() : name;

	if (!std::filesystem::exists(path))
	{
		log(LogLevel::Error, std::format("Could not find plugin '{}' at {}.", pluginName, path.string()));
		removeKnownPlugin(pluginName);
		return false;
	}

	// Already loaded
	if (m_loadedPlugins.contains(pluginName))
	{
		log(LogLevel::Warning, std::format("Plugin '{}' is already loaded.", pluginName));
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
			log(LogLevel::Error, std::format("Failed to load plugin '{}': {}", pluginName, info.errorMessage));

			removeKnownPlugin(pluginName);
			return false;
		}

		//Add descriptor check
		const auto pluginDescriptorFunc = reinterpret_cast<PluginDescriptor*(*)()>(GetSymbol(handle, "getPluginDescriptor"));

		if(!pluginDescriptorFunc)
		{
			info.errorMessage = getError();
			UnloadLibrary(handle);
			log(LogLevel::Error, std::format("Missing 'getPluginDescriptor' function for '{}':{} - Did you add it the dll EXPORT section?", pluginName, info.errorMessage));
			removeKnownPlugin(pluginName);
			return false;
		}

		auto formattedDescriptor = formatPluginDescriptor(pluginDescriptorFunc());
		if(formattedDescriptor.has_value())
		{
			log(LogLevel::Info, formattedDescriptor.value());
		}
		else
		{
			log(LogLevel::Error, formattedDescriptor.error());
		}


		//Check that the context provides the REQUIRED services; otherwise stop trying to load the plugin.
		// We will however, still load plugins if OPTIONAL services are not available



		// Load init symbol
		auto pluginEntryFunction = reinterpret_cast<IPlugin*(*)()>(GetSymbol(handle, "loadPlugin"));
		if (!pluginEntryFunction)
		{
			info.errorMessage = getError();
			UnloadLibrary(handle);
			log(LogLevel::Error, std::format("Missing 'initPlugin' in '{}':{}", pluginName, info.errorMessage));
			return false;
		}


		auto pluginLogger = createPluginLogger(pluginName);

		// Construct plugin and context(currently just one type of Context)
		auto assignedPluginContext = std::make_unique<PluginRuntimeContext>(pluginLogger, m_dataPacketRegistry, pluginName);
		assignedPluginContext->registerService<ILogger>(pluginLogger);
		assignedPluginContext->registerService<IRestClient>(m_restClient); // This might be wrong

		std::unique_ptr<IPlugin> plugin(pluginEntryFunction());

		auto loadResult = plugin->onPluginLoad(*assignedPluginContext);


		// Handle issue loading plugin
		if(!loadResult.has_value())
		{
			info.errorMessage = loadResult.error();
			log(LogLevel::Error, std::format("Plugin '{}' failed to load: {}", pluginName, loadResult.error()));

			plugin->onPluginUnload();
			return false;
		}

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
		log(LogLevel::Info, std::format("Successfully loaded plugin '{}'", pluginName));

		return true;
	}
	catch (const std::exception& ex)
	{
		info.errorMessage = ex.what();
		log(LogLevel::Error, std::format("Exception while loading plugin '{}': {}", pluginName, ex.what()));
		return false;
	}
}

bool PluginManager::unloadPlugin(const std::string& name)
{
	const auto pluginItr = m_loadedPlugins.find(name);
	if(pluginItr != m_loadedPlugins.end())
	{
		// Unload and remove from collection of loaded plugins
		m_loadedPlugins.erase(pluginItr);

		// Update the collection of all known plugins
		auto discovered = m_discoveredPlugins.find(name);
		if(discovered != m_discoveredPlugins.end())
		{
			discovered->second.loaded = false;
		}

		return true;
	}

	log(LogLevel::Error, std::format("Plugin '{}' is not loaded; cannot unload.", name));
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

void PluginManager::log(const LogLevel logLvl, const std::string& msg) const
{
	m_baseLogger.log(logLvl, std::format("[{}] - {}", "PluginManager", msg));
}

std::shared_ptr<ILogger> PluginManager::createPluginLogger(const std::string& pluginName) const
{
	const auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	std::vector<spdlog::sink_ptr> sinks {consoleSink};

	if(m_uiLogSink)
	{
		sinks.push_back(m_uiLogSink);
	}

	auto pluginSpdLogger = std::make_shared<SpdLogger>(pluginName, sinks);

	return std::make_shared<PluginLogger>(pluginSpdLogger, pluginName);
}

std::filesystem::path PluginManager::getPortableConfigPath() const
{
	std::filesystem::path execDir = getExecutableDir();
	std::filesystem::path configDir = execDir / "config";

	// Ensure config directory exists
	std::error_code ec;
	std::filesystem::create_directories(configDir, ec);
	if (ec)
	{
		log(LogLevel::Warning, std::format("Could not create config directory '{}': {}",
			configDir.string(), ec.message()));
		// Fall back to executable directory
		return (execDir / m_configurationFileName);
	}

	return (configDir / m_configurationFileName);
}

void PluginManager::startPluginAutoScan()
{
	log(LogLevel::Info, std::format("Starting plugin folder watcher: scanning '{}' every {}s", m_config.pluginDirectory, m_config.pluginScanInterval.count()));
	if(!m_config.autoScan || m_scanningThread.joinable())
	{
		return;
	}

	// Start a thread that will periodically call scanPluginFolder
	m_scanningThread = std::jthread([this](const std::stop_token& stop_token)
	{
		while (!stop_token.stop_requested())
		{
			scanPluginsFolder(m_config.pluginDirectory);

			std::this_thread::sleep_for(m_config.pluginScanInterval);
		}
	});
}

void PluginManager::stopPluginAutoScan()
{
	log(LogLevel::Info, "Stopping plugin folder watcher.");
	m_scanningThread.request_stop();
}

PluginManagerConfig& PluginManager::getConfig()
{
	return m_config;
}

void PluginManager::setScanInterval(std::chrono::seconds scanInterval)
{
	m_config.pluginScanInterval = scanInterval;
}

std::chrono::seconds PluginManager::getScanInterval() const
{
	return m_config.pluginScanInterval;
}

void PluginManager::setPluginFolderDir(const std::string& dirPath)
{
	m_config.pluginDirectory = dirPath;
}

std::filesystem::path PluginManager::getPluginFolderDir() const
{
	return m_config.pluginDirectory;
}

void PluginManager::setAutoScan(bool bShouldScan)
{
	m_config.autoScan = bShouldScan;
}

bool PluginManager::getAutoScanEnabled() const
{
	return m_config.autoScan;
}

void PluginManager::reloadPluginConfig()
{
	loadConfig();
}

void PluginManager::enablePluginDebugLogging(const std::string& pluginName)
{
	auto it = m_loadedPlugins.find(pluginName);
	if(it != m_loadedPlugins.end())
	{
		it->second->enablePluginDebugLogging();
	}
}

void PluginManager::disablePluginDebugLogging(const std::string& pluginName)
{
	auto it = m_loadedPlugins.find(pluginName);
	if(it != m_loadedPlugins.end())
	{
		it->second->disablePluginDebugLogging();
	}
}

bool PluginManager::isPluginDebugLoggingEnabled(const std::string& pluginName) const
{
	auto it = m_loadedPlugins.find(pluginName);
	if(it != m_loadedPlugins.end())
	{
		return it->second->isPluginDebugLoggingEnabled();
	}

	return false;
}

void PluginManager::tick()
{
	for(const auto& plugin : m_loadedPlugins | std::views::values)
	{
		plugin->tick();
	}
}

//Need to work into things
void PluginManager::registerServicesForPlugin(PluginRuntimeContext* context, const PluginDescriptor* desc) const
{
	// Simple policy logic inline
	if (isTrustedPlugin(desc->name))
	{
		context->registerService<IRestClient>(m_restClient);
		//context->registerService<IFileSystem>(m_fileSystem);
	}

	if (desc->name.starts_with("Debug_"))
	{
		//context->registerService<IDebugger>(m_debugger);
	}

	// Network plugins get REST client
	if (needsNetworkAccess(desc))
	{
		context->registerService<IRestClient>(m_restClient);
	}
}

