#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "../DataPacket/IDataPacketHandler.h"
#include <filesystem>

#include "Logger/ILogger.h"

struct IPlugin;
class PluginRuntimeContext;


#ifdef _WIN32
	#include <windows.h>
class PluginRuntimeContext;
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
/// PluginInstance is used to own a IPlugin, and controls it's lifecycle
/// </summary>

class PluginInstance
{
public:
	PluginInstance(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginRuntimeContext> ctx, std::string name = "");
	~PluginInstance();

	PluginInstance(const PluginInstance&) = delete;
	PluginInstance& operator=(const PluginInstance&) = delete;

	PluginInstance(PluginInstance&&) noexcept;
	PluginInstance& operator=(PluginInstance&& other) noexcept;

	[[nodiscard]] IPlugin*                                    get() const;
	[[nodiscard]] std::expected<std::type_index, std::string> getType() const;
	[[nodiscard]] PluginRuntimeContext*                          getContext() const;

	void tick() const;
	bool unload();

	void enableDebugLogging();
	void disableDebugLogging();
	bool isDebugLoggingEnabled() const ;

private:

	void log(LogLevel level, const std::string& msg) const;
	LibraryHandle m_handle = nullptr;
	std::unique_ptr<IPlugin> m_plugin;
	std::unique_ptr<PluginRuntimeContext> m_context;
	std::string m_pluginName;
	std::shared_ptr<ILogger> m_logger;
};