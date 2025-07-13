#pragma once

#include "Plugin/IPlugin.h"
#include "Logger/LoggablePlugin.h"
#include "DataPacket/IDataPacketHandler.h"

class NumberLoggerHandler : public ITypedDataPacketHandler<int>, public LoggablePlugin
{
public:
	bool handleType(const std::shared_ptr<int>& data) override;
};

class NumberLoggerPlugin : public IPlugin, public LoggablePlugin
{
	public:
		void            onPluginLoad(IPluginContext& context) override;
		void            onPluginUnload() override;
		std::type_index getType() const override;

	private:
		std::shared_ptr<NumberLoggerHandler> m_handler;
};