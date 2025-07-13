#include "GPS_Plugin.h"
#include "DataPacket/IDataPacketHandler.h"
#include "Plugin/PluginRuntimeContext.h"

#include <iostream>

bool GPSDataHandler::handle(const DataPacket& packet)
{
	auto gps = packet.get<GPSDataPacket>();
	if(gps.has_value())
	{
		std::cout << "[GPS] lat:" << gps.value()->lat << ", long:" << gps.value()->lng << ", alt: " << gps.value()->alt << "\n";
		return true;
	}

	std::cout << "[ERROR]: " << gps.error() << "\n";
	return false;
}

void GPSPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLoggerShared(), "GPSPlugin");
	context.registerDataPacketHandler(typeid(GPSDataPacket), std::make_shared<GPSDataHandler>());
	log(LogLevel::Info, "loaded");
}

void GPSPlugin::onPluginUnload()
{
	log(LogLevel::Info, "unloading");
	unsetLogger();
}

std::type_index GPSPlugin::getType() const
{
	return typeid(GPSDataPacket);
}

EXPORT IPlugin* initPlugin()
{
	return new GPSPlugin();
}