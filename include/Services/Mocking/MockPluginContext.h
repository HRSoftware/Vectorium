#pragma once
//#include <unordered_map>

//#include "Plugin/IPlugin.h"
//
//class MockPluginContext : public IPluginContext
//{
//public:
//	explicit MockPluginContext(std::string name = "MockPlugin");
//
//	std::shared_ptr<ILogger> getLoggerShared() override;
//
//
//	bool registerDataPacketHandler(std::type_index packetType, std::shared_ptr<IDataPacketHandler> packetHandler) override
//	{
//		m_handlers[packetType].push_back(packetHandler);
//		return true;
//	}
//
//		// Add unregister?
//
//	// Optional: expose fake handler map for assertions
//	const std::unordered_map<std::type_index, std::vector<std::shared_ptr<IDataPacketHandler>>>& getRegisteredHandlers() const;
//
//private:
//	std::string m_pluginName;
//	std::shared_ptr<ILogger> m_logger;
//	std::unordered_map<std::type_index, std::vector<std::shared_ptr<IDataPacketHandler>>> m_handlers;
//
//	// Inherited via IPluginContext
//	std::string getPluginName() const override;
//};