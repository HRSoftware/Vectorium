#pragma once

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

#include "DataPacket/DataPacket.h"
#include "Plugin/IPlugin.h"
#include "Logger/LoggablePlugin.h"

struct NumberDataPacket
{
	int number;
};

struct NumberHandler final : IDataPacketHandler
{
	bool handle(const DataPacket& packet) override;
};

struct NumberGeneratorPlugin final : public IPlugin, public LoggablePlugin
{
	void onPluginLoad(IPluginContext& context) override;
	void onPluginUnload()override;

	[[nodiscard]] std::type_index getType() const override;
};
