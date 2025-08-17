#pragma once
#include <expected>
#include <memory>
#include <typeindex>
#include <filesystem>

enum class LogLevel;
class ILogger;
class IDataPacketHandler;

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
/// Manages the lifecycle and context of a dynamically loaded plugin instance.
/// </summary>
class PluginInstance
{
public:
	PluginInstance(LibraryHandle h, std::unique_ptr<IPlugin> p, std::unique_ptr<PluginRuntimeContext>, std::string name = "");
	~PluginInstance();

	PluginInstance(const PluginInstance&) = delete;
	PluginInstance& operator=(const PluginInstance&) = delete;

	PluginInstance(PluginInstance&&) = delete;
	PluginInstance& operator=(PluginInstance&& other) = delete;

	[[nodiscard]] IPlugin* getPlugin() const;
	//[[nodiscard]] std::expected<std::type_index, std::string> getType() const;
	[[nodiscard]] PluginRuntimeContext* getContext() const;
	const std::string& getPluginName();

	void enabledPluginDebugLogging();
	void disablePluginDebugLogging();
	bool isPluginDebugLoggingEnabled() const;

	void tick() const;

private:

	LibraryHandle m_handle = nullptr;
	std::unique_ptr<IPlugin> m_plugin;
	std::unique_ptr<PluginRuntimeContext> m_context;
	std::string m_pluginName;

	std::chrono::steady_clock::time_point m_lastPluginUpdate;
	std::chrono::seconds m_pluginUpdateInterval{ 5 };
};