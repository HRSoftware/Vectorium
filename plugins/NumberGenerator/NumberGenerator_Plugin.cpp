#include "NumberGenerator_Plugin.h"

bool NumberHandler::handle(const DataPacket& packet)
{
	auto numberPacket = packet.get<NumberDataPacket>();
	if (numberPacket.has_value())
	{
		std::cout << std::format("Number processed: {}\n", numberPacket.value()->number);
		return true;
	}

	std::cout << std::format("[ERROR]: {}\n", numberPacket.error());
	return false;
}

void NumberGeneratorPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLoggerShared(), "NumberGeneratorPlugin");
	context.registerDataPacketHandler(typeid(NumberGeneratorPlugin), std::make_shared<NumberHandler>());
	log(LogLevel::Info, "loaded");
}

void NumberGeneratorPlugin::onPluginUnload()
{
	log(LogLevel::Info, "unloading");
	unsetLogger();
}

std::type_index NumberGeneratorPlugin::getType() const
{
	return  typeid(NumberDataPacket);;
}

EXPORT IPlugin* initPlugin()
{
	return new NumberGeneratorPlugin();
}