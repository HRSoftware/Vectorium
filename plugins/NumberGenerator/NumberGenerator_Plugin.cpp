#include "NumberGenerator_Plugin.h"

#include <iostream>

#include "Services/IServiceSpecialisations.h"

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

NumberGeneratorPlugin::NumberGeneratorPlugin() = default;

std::expected<void, std::string> NumberGeneratorPlugin::onPluginLoad(IPluginContext& context)
{
	m_logger = ServiceProxy(context.getService<ILogger>());
	m_logger->setPluginName(context.getPluginName());
	m_context = &context;
	// No need to register a handler as we don't want to do anything

	m_logger->log(LogLevel::Info, "loaded");
	return {};
}

void NumberGeneratorPlugin::onPluginUnload()
{
	m_logger->log(LogLevel::Info, "unloading");
}

std::type_index NumberGeneratorPlugin::getType() const
{
	return  typeid(NumberDataPacket);
}

void NumberGeneratorPlugin::tick()
{
	int value = m_dist(m_rng);

	const DataPacket packet = DataPacket::create(std::make_shared<int>(value));

	if (m_context)
	{
		m_context->dispatch(packet);
	}
}

EXPORT PluginDescriptor* getPluginDescriptor()
{
	static PluginDescriptor descriptor
	{
		.name = "NumberGenerator",
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
	return new NumberGeneratorPlugin();
}