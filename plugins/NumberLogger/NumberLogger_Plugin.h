#pragma once

#include "Plugin/IPlugin.h"
#include "DataPacket/IDataPacketHandler.h"
#include "Services/IService.h"

class NumberLoggerHandler : public ITypedDataPacketHandler<int>
{
public:
	NumberLoggerHandler(ServiceProxy<ILogger> logger);
	bool handleType(const std::shared_ptr<int>& data) override;
	~NumberLoggerHandler() override = default;

private:
		ServiceProxy<ILogger> m_logger{ nullptr };
};

class NumberLoggerPlugin : public IPlugin
{
	public:
		std::expected<void, std::string> onPluginLoad(IPluginContext& context) override;
		void            onPluginUnload() override;
		std::type_index getType() const override;

	private:
		std::shared_ptr<NumberLoggerHandler> m_handler;
		ServiceProxy<ILogger> m_logger {nullptr};
};