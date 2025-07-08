#pragma once

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif
#include "Plugin/IPlugin.h"
#include "Logger/LoggablePlugin.h"

struct GPSData
{
	float lat;
	float lng;
	float alt;
};

struct GPSPlugin final : public IPlugin, public LoggablePlugin
{
	void onPluginLoad(IPluginContext& context) override;
	void onPluginUnload()override;

	[[nodiscard]] std::type_index getType() const override;
};
