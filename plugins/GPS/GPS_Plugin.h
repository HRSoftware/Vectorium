#pragma once


#include "Plugin/IPlugin.h"
#include "Logger/LoggablePlugin.h"

struct GPSDataPacket
{
	float lat;
	float lng;
	float alt;
};

struct GPSDataHandler final : ITypedDataPacketHandler<GPSDataPacket>, public LoggablePlugin
{
	bool handleType(const std::shared_ptr<GPSDataPacket>& packet) override;
};

struct GPSPlugin final : public IPlugin, public LoggablePlugin
{
	void onPluginLoad(IPluginContext& context) override;
	void onPluginUnload()override;

	[[nodiscard]] std::type_index getType() const override;
};
