#pragma once


#include "Plugin/IPlugin.h"
#include "Services/IService.h"

class ILogger;

struct GPSDataPacket
{
	float lat;
	float lng;
	float alt;
};

struct GPSDataHandler final : ITypedDataPacketHandler<GPSDataPacket>
{
	bool handleType(const std::shared_ptr<GPSDataPacket>& packet) override;
};

struct GPSPlugin final : public IPlugin
{
	std::expected<void, std::string> onPluginLoad(IPluginContext& context) override;
	void onPluginUnload()override;

	ServiceProxy<ILogger> m_Logger{nullptr};

	[[nodiscard]] std::type_index getType() const override;
	void                          tick() override;
};
