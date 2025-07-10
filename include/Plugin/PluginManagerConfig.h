#pragma once

struct PluginManagerConfig
{
	bool autoScan = true;
	std::string pluginDirectory = "plugins";
	std::chrono::seconds pluginScanInterval = std::chrono::seconds(5);
	std::vector<std::string> enabledPluginsOnStartup;
};