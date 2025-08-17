#pragma once

#include <set>
#include <shared_mutex>
#include <mutex>

#include "IPlugin.h"

class ILogger;
class DataPacketRegistry;

class PluginRuntimeContext : public IPluginContext
{
public:
	PluginRuntimeContext(std::shared_ptr<ILogger> pluginLogger
						,DataPacketRegistry& registry
						,std::string pluginName);

	template<typename T>
	void registerService(std::shared_ptr<T> service);

	template<typename T>
	void unregisterService();

	std::shared_ptr<ILogger> getLoggerShared() override;

	void        unregisterDataPacketHandler();
	std::string getPluginName() const override;
	void        log(LogLevel level, const std::string& message) const override;

private:
	DataPacketRegistry& m_registry;
	std::shared_ptr<ILogger> m_pluginLogger;
	std::string m_pluginName;

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_services;
	mutable std::shared_mutex m_mutex;

	protected:
		bool registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> handler) override;

public:
	void                         dispatch(const DataPacket& packet) override;

private:
	std::expected<std::shared_ptr<void>, std::string> getServiceByTypeIndex(std::type_index tIdx) override;
	bool hasServiceByTypeIndex(std::type_index tIdx) const override;
};

template <typename T> void PluginRuntimeContext::registerService(std::shared_ptr<T> service)
{
	std::unique_lock lk(m_mutex);
	m_services[std::type_index(typeid(T))] = std::static_pointer_cast<void>(service);
}

template <typename T> void PluginRuntimeContext::unregisterService()
{
	std::unique_lock lock(m_mutex);
	m_services.erase(std::type_index(typeid(T)));
}
