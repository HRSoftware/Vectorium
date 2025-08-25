#pragma once

#include <functional>
#include <typeindex>
#include "IPluginContext.h"
#include <Services/ServiceId.h>

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C" __attribute__((visibility("default")))
#endif

enum class EngineEventType;

// Used to indicate that services the plugin needs ie REST, logging, etc
struct PluginDescriptor
{
	enum class SecurityLevel
	{
		Protected,
		Standard,
		Extra,
		Admin
	};

	std::string name;
	std::string version;
	std::vector<ServiceId> services; // required + optional
	SecurityLevel requestedSecurityLevel = SecurityLevel::Standard;
};

struct IPlugin
{
	virtual std::expected<void, std::string> onPluginLoad(IPluginContext& context) = 0;
	virtual void onPluginUnload() = 0;

	virtual void            tick() {}
	virtual void            onDataPacket(const DataPacket& packet) {}
	virtual void            onEngineEvent(const EngineEventType& event) {}
	virtual std::type_index getType() const = 0;

	virtual ~IPlugin() = default;


	// UI Related stuff
	virtual bool        hasUIWindow() const { return false; }
	virtual std::string getUIWindowTitle() const { return "Plugin Window"; }
	virtual bool        isUIWindowVisible() const { return false; }
	virtual void        setUIWindowVisible(bool visible) {}
	virtual void        toggleUIWindow() { }
	//virtual void        renderPluginUI() {} // optional UI overload

	PluginDescriptor getPluginDescriptor()
	{
		return m_plugin_descriptor;
	}


	PluginDescriptor m_plugin_descriptor;
};

