#pragma once

#include <set>
#include <shared_mutex>
#include <mutex>

#include "IPlugin.h"
#include "Services/ServiceContainer.h"
class ILogger;
class DataPacketRegistry;

class PluginRuntimeContext : public IPluginContext
{
public:
	PluginRuntimeContext(std::shared_ptr<ILogger> pluginLogger
		                     , DataPacketRegistry&    registry
		                     , std::string            pluginName
		                     , ServiceContainer&      services );

	template<typename T>
	void registerService(const std::shared_ptr<T>& service);

	template<typename T>
	void unregisterService();

	std::shared_ptr<ILogger> getLoggerShared() override;

	void        unregisterDataPacketHandler();
	std::string getPluginName() const override;
	void        log(LogLevel level, const std::string& message) const override;

	private:
		void populateServices();

private:
	DataPacketRegistry& m_dataPacketRegistry;
	std::shared_ptr<ILogger> m_pluginLogger;
	std::string m_pluginName;

	ServiceContainer& m_services;
	std::unordered_map<std::type_index, std::shared_ptr<void>> m_localServices;
	mutable std::shared_mutex m_mutex;

	std::function<void*()> m_getImGuiContextFunc;
	std::function<bool(void*)> m_setImGuiContextFunc;

protected:
	bool registerDataPacketHandler(std::type_index type, std::shared_ptr<IDataPacketHandler> handler) override;

public:
	void                         dispatch(const DataPacket& packet) override;

private:
	std::expected<std::shared_ptr<void>, std::string> getServiceByTypeIndex(std::type_index tIdx) override;
	bool hasServiceByTypeIndex(std::type_index tIdx) const override;

public:
	void* getMainAppImGuiContext() override;
	bool  setImGuiContext(void* ctx) override;
	void  setImGuiContextFunctions(std::function<void*()> getFunc, std::function<bool(void*)> setFunc);
};

template <typename T> void PluginRuntimeContext::registerService(const std::shared_ptr<T>& service)
{
	std::unique_lock lk(m_mutex);
	m_localServices[std::type_index(typeid(T))] = service;
}

template <typename T> void PluginRuntimeContext::unregisterService()
{
	std::unique_lock lock(m_mutex);
	m_localServices.erase(std::type_index(typeid(T)));
}
