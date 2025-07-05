#pragma once
#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class LoadedPlugin;



/// <summary>
/// PluginManager is used to track and handle the lifetime of all loaded plugins
/// </summary>
class PluginManager
{
public:
		void loadPluginsFolder();
		bool loadPlugin(const std::filesystem::path& path, const std::string& = "");

		[[nodiscard]] std::vector<std::string> getLoadedPluginNames() const;

		//void reloadPlugin(const std::string name);

private:
	std::vector<std::unique_ptr<LoadedPlugin>> loadedPlugins;
};
