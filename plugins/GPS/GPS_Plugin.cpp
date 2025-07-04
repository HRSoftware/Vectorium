#include "GPS_Plugin.h"
#include "DataPacket/IDataHandler.h"
#include "Plugin/PluginContextImpl.h"

#include <iostream>

struct GPSDataHandler final : IDataHandler
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

struct GPSPlugin final : IPlugin
{
	void registerType(IPluginContext& context) override
	{
		context.registerHandler(typeid(GPSData), []
		{
			return std::make_unique<GPSDataHandler>();
		});
	}

	std::type_index getType() const override
	{
		return typeid(GPSData);
	};
};


EXPORT IPlugin* initPlugin()
{
	return new GPSPlugin();
}