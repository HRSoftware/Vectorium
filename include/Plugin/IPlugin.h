#pragma once

#include <functional>
#include <typeindex>
#include "IPluginContext.h"

/// <summary>
/// 
/// </summary>
struct IPlugin
{
	virtual void onPluginLoad(IPluginContext& context) = 0;
	virtual void onPluginUnload() = 0;
	virtual std::type_index getType() const = 0;
	virtual ~IPlugin() = default;
};