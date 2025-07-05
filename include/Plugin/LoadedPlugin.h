#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "../DataPacket/IDataHandler.h"
#include <filesystem>

struct IPlugin;
class PluginContextImpl;


#ifdef _WIN32
	#include <windows.h>
class PluginContextImpl;
	using LibraryHandle = HMODULE;
	#define LoadSharedLibrary(path) LoadLibraryA(path)
	#define GetSymbol(handle, name) GetProcAddress(handle, name)
	#define UnloadLibrary(handle)   FreeLibrary(handle)
	#define getError() std::to_string(GetLastError())
	constexpr auto PLUGIN_EXT = ".dll";
#else
	#include <dlfcn.h>
	using LibraryHandle = void*;
	#define LoadSharedLibrary(path) dlopen(path, RTLD_NOW)
	#define GetSymbol(handle, name) dlsym(handle, name)
	#define UnloadLibrary(handle)   dlclose(handle)
	#define getError() dlerror()
	constexpr auto PLUGIN_EXT = ".so";
#endif


/// <summary>
/// LoadPlugin is used to own a IPlugin, and controls it's lifecycle
/// </summary>

class LoadedPlugin
{
public:
	LoadedPlugin(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginContextImpl> ctx, std::string name = "");
	~LoadedPlugin();

	LoadedPlugin(const LoadedPlugin&) = delete;
	LoadedPlugin& operator=(const LoadedPlugin&) = delete;

	LoadedPlugin(LoadedPlugin&&) noexcept;
	LoadedPlugin& operator=(LoadedPlugin&& other) noexcept;

	[[nodiscard]] IPlugin*                                    get() const;
	[[nodiscard]] std::expected<std::type_index, std::string> getType() const;
	[[nodiscard]] PluginContextImpl*                          getContext() const;

private:
	LibraryHandle handle = nullptr;
	std::unique_ptr<IPlugin> plugin;
	std::unique_ptr<PluginContextImpl> context;
	std::string pluginName;

	void unload();
};