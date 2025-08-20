
#include <cassert>
#include <format>
#include <utility>
#include "Plugin/PluginRuntimeContext.h"

#include "imgui.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/SpdLogger.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Services/IServiceSpecialisations.h"


PluginRuntimeContext::PluginRuntimeContext(std::shared_ptr<ILogger> pluginLogger, DataPacketRegistry& registry, std::string pluginName):
m_registry(registry)
, m_pluginLogger(std::move(pluginLogger))
, m_pluginName(std::move(pluginName))
{
}

std::shared_ptr<ILogger> PluginRuntimeContext::getLoggerShared()
{
	return m_pluginLogger;
}

bool PluginRuntimeContext::registerDataPacketHandler(const std::type_index type, std::shared_ptr<IDataPacketHandler> handler)
{
	log(LogLevel::Info, std::format("Registered handle for {}", type.name()));
	return m_registry.registerDataPacketHandler(type, std::move(handler), m_pluginName);
}

void PluginRuntimeContext::dispatch(const DataPacket& packet)
{
	if (!packet.payload) return;

	m_registry.dispatch(packet);
}

std::expected<std::shared_ptr<void>, std::string> PluginRuntimeContext::getServiceByTypeIndex(std::type_index tIdx)
{
	std::shared_lock lock(m_mutex);

	if (const auto it = m_services.find(tIdx); it != m_services.end() && it->second)
	{
		return it->second;
	}

	return std::unexpected("Service not found or unavailable");
}

bool PluginRuntimeContext::hasServiceByTypeIndex(std::type_index tIdx) const
{
	std::shared_lock lock(m_mutex);
	const auto it = m_services.find(tIdx);
	return it != m_services.end() && it->second != nullptr;
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

void PluginRuntimeContext::uiSetNextWindowSize(float width, float height, int cond)
{
	// Call main app's ImGui through the context functions
	if (m_getImGuiContextFunc && m_setImGuiContextFunc)
	{
		void* ctx = m_getImGuiContextFunc();
		if (ctx && m_setImGuiContextFunc(ctx))
		{
			ImGui::SetNextWindowSize(ImVec2(width, height), cond);
		}
	}
}

bool PluginRuntimeContext::uiBegin(const char* name, bool* p_open)
{
	if (m_getImGuiContextFunc && m_setImGuiContextFunc)
	{
		void* ctx = m_getImGuiContextFunc();
		if (ctx && m_setImGuiContextFunc(ctx))
		{
			return ImGui::Begin(name, p_open);
		}
	}
	return false;
}

void PluginRuntimeContext::uiEnd()
{
	if (m_getImGuiContextFunc && m_setImGuiContextFunc)
	{
		void* ctx = m_getImGuiContextFunc();
		if (ctx && m_setImGuiContextFunc(ctx))
		{
			ImGui::End();
		}
	}
}

void PluginRuntimeContext::uiText(const char* text)
{
	if (m_getImGuiContextFunc && m_setImGuiContextFunc)
	{
		void* ctx = m_getImGuiContextFunc();
		if (ctx && m_setImGuiContextFunc(ctx))
		{
			ImGui::Text("%s", text);
		}
	}
}

bool PluginRuntimeContext::uiButton(const char* label)
{
	if (m_getImGuiContextFunc && m_setImGuiContextFunc)
	{
		void* ctx = m_getImGuiContextFunc();
		if (ctx && m_setImGuiContextFunc(ctx))
		{
			return ImGui::Button(label);
		}
	}
	return false;
}

//Needed? Move to base class?
void PluginRuntimeContext::unregisterDataPacketHandler()
{
	m_registry.unregisterDataPacketHandlerForPlugin(m_pluginName);
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