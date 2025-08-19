#include "GPS_Plugin.h"
#include "Services/IServiceSpecialisations.h"

bool GPSDataHandler::handleType(const std::shared_ptr<GPSDataPacket>& packet)
{
	//log(LogLevel::Info, std::format("lat:{}, long:{}, alt:{}", packet->lat, packet->lng, packet->alt));
	return false;
}

void GPSPlugin::onPluginLoad(IPluginContext& context)
{
	m_Logger = ServiceProxy(context.getService<ILogger>());

	// Registering our interest in GPSDataPackets
	context.registerTypedHandler<GPSDataPacket>(std::make_shared<GPSDataHandler>());
	m_Logger->log(LogLevel::Info, "Loaded");
}

void GPSPlugin::onPluginUnload()
{

	//log(LogLevel::Info, "unloading");
	
	//unsetLogger();
}

std::type_index GPSPlugin::getType() const
{
	return typeid(GPSDataPacket);
}

void GPSPlugin::tick()
{
	m_Logger->log(LogLevel::Debug, "Tick");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT PluginDescriptor* getPluginDescriptor()
{
	static PluginDescriptor descriptor
	{
		.name = "GPS",
		.version = "1.0.0",
		.services = {
			{
				.type = typeid(ILogger),
				.name = "logger",
				.minVersion = ">=1.0.0",
				.required = false
			}
		}
	};

	return &descriptor;
}

EXPORT IPlugin* loadPlugin()
{
	return new GPSPlugin();
}