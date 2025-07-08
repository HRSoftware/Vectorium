#include "GPS_Plugin.h"
#include "DataPacket/IDataPacketHandler.h"
#include "Plugin/PluginContextImpl.h"

#include <iostream>

struct GPSDataHandler final : IDataPacketHandler
{
	void handle(const DataPacket& packet) override
	{
		auto gps = packet.get<GPSData>();
		if(gps.has_value())
		{
			std::cout << "[GPS] lat:" << gps.value()->lat << ", long:" << gps.value()->lng << ", alt: " << gps.value()->alt << "\n";
		}
		else
		{
			std::cout << "[ERROR]: " << gps.error() << "\n";
		}
	}
};



void GPSPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLogger(), "GPSPlugin");
	context.registerDataPacketHandler(typeid(GPSData), std::make_shared<GPSDataHandler>());
	log(LogLevel::Info, "loaded");
}

void GPSPlugin::onPluginUnload()
{
	log(LogLevel::Info, "unloading");
}

std::type_index GPSPlugin::getType() const
{
	return typeid(GPSData);
}

EXPORT IPlugin* initPlugin()
{
	return new GPSPlugin();
}