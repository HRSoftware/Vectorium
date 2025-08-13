#pragma once

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

#include <random>

#include "DataPacket/DataPacket.h"
#include "Plugin/IPlugin.h"
#include "Services/IService.h"

struct NumberDataPacket
{
	int number;
};

struct NumberHandler final : IDataPacketHandler
{
	bool handle(const DataPacket& packet) override;
};

struct NumberGeneratorPlugin final : public IPlugin
{
	NumberGeneratorPlugin();
	void onPluginLoad(IPluginContext& context) override;
	void onPluginUnload() override;

	[[nodiscard]] std::type_index getType() const override;
	void                          onPluginTick() override;

	private:
		IPluginContext* m_context;
		ServiceProxy<ILogger> m_logger{nullptr};
		std::mt19937 m_rng{std::random_device{}()};
		std::uniform_int_distribution<int> m_dist{0, 100};
		std::string m_pluginName = "NumberGenerator";
};
