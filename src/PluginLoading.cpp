#include <iostream>
#include <string>
#include <vector>
#include <format>
//#include "../include/Plugin/PluginLoading.h"
#include "../include/Plugin/PluginContextImpl.h"
#include "../include/Plugin/IPlugin.h"
#include "../include/Plugin/LoadedPlugin.h"

//std::expected<LoadedPlugin, std::string> tryLoadPlugin(const std::filesystem::path& pluginPath, std::vector<LoadedPlugin>& vec) noexcept
//{
//	if(!std::filesystem::exists(pluginPath))
//	{
//		std::cout << std::format("Could not find plugin at '{}'", pluginPath.string());
//		return;
//	}
//
//	for (const auto& entry : std::filesystem::directory_iterator(pluginPath)) 
//	{
//		if (!entry.is_regular_file()) continue;
//		if (entry.path().extension() != PLUGIN_EXT) continue;
//
//		std::string path = entry.path().string();
//
//		const LibraryHandle handle = LoadSharedLibrary(path.c_str());
//		if (!handle) 
//		{
//			std::cout << std::format("Failed to load plugin '{}' - ({})\n", path, getError());
//			return;
//		}
//
//		const auto create = reinterpret_cast<IPlugin * (*)()> (GetSymbol(handle, "initPlugin"));
//		if (!create) 
//		{
//			UnloadLibrary(handle);
//			std::cout << std::format("Could not find 'initPlugin' entry point in plugin '{}' - ({})\n", path, getError());
//			return;
//		}
//
//		std::unique_ptr<IPlugin> plugin(create());
//		auto                     context = std::make_unique<PluginContextImpl>();
//		plugin->registerType(*context);
//		vec.emplace_back(handle, std::move(plugin), std::move(context));
//	}
//}
