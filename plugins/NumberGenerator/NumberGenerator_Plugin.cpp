#include "NumberGenerator_Plugin.h"

#include "Plugin/PluginRuntimeContext.h"

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

NumberGeneratorPlugin::NumberGeneratorPlugin()
{
	m_pluginName = "NumberGenerator";
}

void NumberGeneratorPlugin::onPluginLoad(IPluginContext& context)
{
	setLogger(context.getLoggerShared(), "NumberGeneratorPlugin");
	m_context = &context;
	// No need to register a handler as we don't want to do anything

	log(LogLevel::Info, "loaded");
}

void NumberGeneratorPlugin::onPluginUnload()
{
	log(LogLevel::Info, "unloading");
	unsetLogger();
}

std::type_index NumberGeneratorPlugin::getType() const
{
	return  typeid(NumberDataPacket);
}

void NumberGeneratorPlugin::onPluginTick()
{
	int value = m_dist(m_rng);

	const DataPacket packet = DataPacket::create(std::make_shared<int>(value));

	if (m_context)
	{
		m_context->dispatch(packet);
	}
}

EXPORT IPlugin* initPlugin()
{
	return new NumberGeneratorPlugin();
}