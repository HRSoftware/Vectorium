#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "IDataHandler.h"
struct IPlugin;
class PluginContextImpl;


#ifdef _WIN32
	#include <windows.h>
class PluginContextImpl;
	using LibraryHandle = HMODULE;
	#define LoadSharedLibrary(name) LoadLibraryA(name)
	#define GetSymbol(handle, name) GetProcAddress(handle, name)
	#define UnloadLibrary(handle)   FreeLibrary(handle)
	#define getError() std::to_string(GetLastError())
#else
	#include <dlfcn.h>
	using LibraryHandle = void*;
	#define LoadSharedLibrary(name) dlopen(name, RTLD_NOW)
	#define GetSymbol(handle, name) dlsym(handle, name)
	#define UnloadLibrary(handle)   dlclose(handle)
	#define getError() dlerror()
#endif


class LoadedPlugin
{
public:
	LoadedPlugin(const std::string& path);
	LoadedPlugin(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginContextImpl> ctx);
	~LoadedPlugin();

	LoadedPlugin(const LoadedPlugin&) = delete;
	LoadedPlugin& operator=(const LoadedPlugin&) = delete;

	LoadedPlugin(LoadedPlugin&&) noexcept;
	LoadedPlugin& operator=(LoadedPlugin&& other) noexcept;

	IPlugin* get() const;
	std::expected<std::type_index, std::string> getType() const;
	PluginContextImpl* getContext() const;

private:
	LibraryHandle handle = nullptr;
	std::unique_ptr<IPlugin> plugin;
	std::unique_ptr<PluginContextImpl> context;

	void unload();
};

void tryLoadPlugin(const std::string& path, std::vector<LoadedPlugin>& vec) noexcept;
