
#include <cassert>
#include <format>
#include <utility>
#include "Plugin/PluginRuntimeContext.h"

#include "Services/Logging/ILogger.h"
#include "Services/Logging/SpdLogger.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/IServiceSpecialisations.h"


PluginRuntimeContext::PluginRuntimeContext(
	std::shared_ptr<ILogger> pluginLogger
	, DataPacketRegistry& registry
	, std::string pluginName
	, ServiceContainer& services):
m_dataPacketRegistry(registry)
, m_pluginLogger(std::move(pluginLogger))
, m_pluginName(std::move(pluginName))
, m_services(services)
{
	populateServices();
}

void PluginRuntimeContext::registerServiceByType(std::type_index typeIdx, std::shared_ptr<void> service)
{
	std::unique_lock lk(m_mutex);
	m_localServices[typeIdx] = std::move(service);
}

void PluginRuntimeContext::unregisterServiceByType(std::type_index typeIdx)
{
	std::unique_lock lock(m_mutex);
	m_localServices.erase(typeIdx);
}

std::shared_ptr<ILogger> PluginRuntimeContext::getLoggerShared()
{
	return m_pluginLogger;
}

bool PluginRuntimeContext::registerDataPacketHandler(const std::type_index type, std::shared_ptr<IDataPacketHandler> handler)
{
	log(LogLevel::Info, std::format("Registered handle for {}", type.name()));
	return m_dataPacketRegistry.registerDataPacketHandler(type, std::move(handler), m_pluginName);
}

void PluginRuntimeContext::dispatch(const DataPacket& packet)
{
	if (!packet.payload) return;

	m_dataPacketRegistry.dispatch(packet);
}

std::expected<std::shared_ptr<void>, std::string> PluginRuntimeContext::getServiceByTypeIndex(std::type_index tIdx)
{
	// First check local services (plugin-specific)
	auto it = m_localServices.find(tIdx);
	if (it != m_localServices.end())
	{
		return it->second;
	}

	// Fallback to global service container
	if (auto service = m_services.getServiceByType(tIdx))
	{
		return service;
	}

	return std::unexpected("Service not found");
}

bool PluginRuntimeContext::hasServiceByTypeIndex(std::type_index tIdx) const
{
	std::shared_lock lock(m_mutex);
	return m_services.hasServiceByType(tIdx);
}

void* PluginRuntimeContext::getMainAppImGuiContext()
{
	if (m_getImGuiContextFunc)
	{
		return m_getImGuiContextFunc();
	}

	log(LogLevel::Warning, "No ImGui context function available");
	return nullptr;
}

bool PluginRuntimeContext::setImGuiContext(void* ctx)
{
	if (m_setImGuiContextFunc)
	{
		return m_setImGuiContextFunc(ctx);
	}
	log(LogLevel::Warning, "No ImGui set context function available");
	return false;
}

void PluginRuntimeContext::setImGuiContextFunctions(std::function<void*()> getFunc, std::function<bool(void*)> setFunc)
{
	m_getImGuiContextFunc = getFunc;
	m_setImGuiContextFunc = setFunc;
	log(LogLevel::Debug, "ImGui context functions set in plugin runtime context");
}

//Needed? Move to base class?
void PluginRuntimeContext::unregisterDataPacketHandler()
{
	m_dataPacketRegistry.unregisterDataPacketHandlerForPlugin(m_pluginName);
	log(LogLevel::Info, std::format("Unregistered handles for {}", m_pluginName));
}

std::string PluginRuntimeContext::getPluginName() const
{
	return m_pluginName;
}

void PluginRuntimeContext::log(LogLevel level, const std::string& message) const
{
	m_pluginLogger->log(level, message);
}

void PluginRuntimeContext::populateServices()
{
		// Store references to services that plugins commonly need
		if (auto logger = m_services.getService<ILogger>())
		{
			registerService<ILogger>(logger);
		}

		if (auto restClient = m_services.getService<IRestClient>())
		{
			registerService<IRestClient>(restClient);
		}

		if(auto uiService = m_services.getService<IUIService>())
		{
			registerService<IUIService>(uiService);
		}
		// Add other services as needed
}

