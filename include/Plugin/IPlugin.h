#pragma once

#include <functional>
#include <typeindex>
#include "IPluginContext.h"

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

/// <summary>
/// 
/// </summary>
struct IPlugin
{
	virtual void onPluginLoad(IPluginContext& context) = 0;
	virtual void onPluginUnload() = 0;

	virtual void onPluginTick() {};
	virtual std::type_index getType() const = 0;
	virtual ~IPlugin() = default;

	std::string m_pluginName;
};