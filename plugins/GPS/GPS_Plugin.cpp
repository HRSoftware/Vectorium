#include "GPS_Plugin.h"
#include "DataPacket/IDataPacketHandler.h"
#include "Plugin/PluginRuntimeContext.h"

#include <iostream>
#include <format>


bool GPSDataHandler::handleType(const std::shared_ptr<GPSDataPacket>& packet)
{
	log(LogLevel::Info, std::format("lat:{}, long:{}, alt:{}", packet->lat, packet->lng, packet->alt));
	return false;
}

void GPSPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLoggerShared(), "GPSPlugin");

	// Registering our interest in GPSDataPackets
	context.registerTypedHandler<GPSDataPacket>(std::make_shared<GPSDataHandler>());

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