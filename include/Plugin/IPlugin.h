#pragma once

#include <functional>
#include <typeindex>
#include "IPluginContext.h"
#include <Services/ServiceId.h>

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

// Used to indicate that services the plugin needs ie REST, logging, etc
struct PluginDescriptor
{
	const char* name;
	const char* version;
	std::vector<ServiceId> services; // required + optional
};

struct IPlugin
{
	virtual void onPluginLoad(IPluginContext& context) = 0;
	virtual void onPluginUnload() = 0;

	virtual void onPluginTick() {};
	virtual std::type_index getType() const = 0;
	virtual void onRender() {}; // optional UI overload
	virtual ~IPlugin() = default;

	PluginDescriptor getPluginDescriptor()
	{
		return m_plugin_descriptor;
	}


	PluginDescriptor m_plugin_descriptor;
};

