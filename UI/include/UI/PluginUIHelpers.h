#pragma once

#include "Services/UI/IPluginUIService.h"
#include "Services/IService.h"
#include <functional>
#include <memory>
#include <imgui.h>

/// <summary>
/// Convenience macros and helpers for plugin UI development
/// Simplifies common UI operations for plugin developers
/// </summary>

// Macro to safely register plugin UI with error checking
#define REGISTER_PLUGIN_UI(context, renderFunction) \
    do { \
        if (auto uiService = (context).getService<IPluginUIService>()) { \
            if (uiService->isAvailable()) { \
                uiService->registerPluginUIRenderer((context).getPluginName(), [this]() { renderFunction(); }); \
            } \
        } \
    } while(0)

// Macro to safely execute ImGui code with context management
#define SAFE_IMGUI_CALL(uiService, code) \
    do { \
        if ((uiService) && (uiService)->isContextValid()) { \
            (uiService)->executeWithContext([&]() { code; }, __FUNCTION__); \
        } \
    } while(0)

/// <summary>
/// RAII helper class for plugin UI management
/// Automatically registers/unregisters UI on construction/destruction
/// </summary>
class PluginUIManager
{
public:
	PluginUIManager(std::shared_ptr<IPluginUIService> uiService, 
		const std::string& pluginName,
		std::function<void()> renderCallback)
		: m_uiService(std::move(uiService))
		, m_pluginName(pluginName)
		, m_registered(false)
	{
		if (m_uiService && m_uiService->isContextValid()) {
			m_uiService->registerPluginUIRenderer(m_pluginName, std::move(renderCallback));
			m_registered = true;
		}
	}

	~PluginUIManager()
	{
		if (m_registered && m_uiService) {
			m_uiService->unregisterPluginUIRenderer(m_pluginName);
		}
	}

	// Non-copyable, movable
	PluginUIManager(const PluginUIManager&) = delete;
	PluginUIManager& operator=(const PluginUIManager&) = delete;

	PluginUIManager(PluginUIManager&& other) noexcept
		: m_uiService(std::move(other.m_uiService))
		, m_pluginName(std::move(other.m_pluginName))
		, m_registered(other.m_registered)
	{
		other.m_registered = false;
	}

	PluginUIManager& operator=(PluginUIManager&& other) noexcept
	{
		if (this != &other) {
			if (m_registered && m_uiService) {
				m_uiService->unregisterPluginUIRenderer(m_pluginName);
			}

			m_uiService = std::move(other.m_uiService);
			m_pluginName = std::move(other.m_pluginName);
			m_registered = other.m_registered;
			other.m_registered = false;
		}
		return *this;
	}

	bool isRegistered() const { return m_registered; }

private:
	std::shared_ptr<IPluginUIService> m_uiService;
	std::string m_pluginName;
	bool m_registered;
};

/// <summary>
/// Utility functions for common plugin UI operations
/// </summary>
namespace PluginUIUtils
{
	/// <summary>
	/// Creates a simple status window for a plugin
	/// </summary>
	inline void RenderSimpleStatusWindow(const std::string& pluginName, 
		const std::string& status,
		bool* open = nullptr)
	{
		if (open && !*open) return;

		std::string windowName = pluginName + " Status";
		if (ImGui::Begin(windowName.c_str(), open)) {
			ImGui::Text("Plugin: %s", pluginName.c_str());
			ImGui::Separator();
			ImGui::Text("Status: %s", status.c_str());
		}
		ImGui::End();
	}

	/// <summary>
	/// Creates a collapsing header section for plugin data
	/// </summary>
	inline bool BeginPluginSection(const std::string& sectionName, bool defaultOpen = true)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_CollapsingHeader;
		if (defaultOpen) {
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		return ImGui::CollapsingHeader(sectionName.c_str(), flags);
	}

	/// <summary>
	/// Displays a simple data table for plugin information
	/// </summary>
	template<typename T>
	void DisplayDataTable(const std::string& tableName, 
		const std::vector<std::pair<std::string, T>>& data)
	{
		if (ImGui::BeginTable(tableName.c_str(), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Property");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();

			for (const auto& [key, value] : data) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%s", key.c_str());
				ImGui::TableNextColumn();

				if constexpr (std::is_same_v<T, std::string>) {
					ImGui::Text("%s", value.c_str());
				} else if constexpr (std::is_arithmetic_v<T>) {
					if constexpr (std::is_floating_point_v<T>) {
						ImGui::Text("%.3f", static_cast<double>(value));
					} else {
						ImGui::Text("%lld", static_cast<long long>(value));
					}
				} else {
					ImGui::Text("Unknown Type");
				}
			}
			ImGui::EndTable();
		}
	}

	/// <summary>
	/// Shows UI service diagnostic information
	/// </summary>
	inline void ShowUIServiceDiagnostics(std::shared_ptr<IPluginUIService> uiService)
	{
		if (!uiService) {
			ImGui::Text("No UI Service Available");
			return;
		}

		if (ImGui::CollapsingHeader("UI Service Diagnostics")) {
			ImGui::Text("Context Valid: %s", uiService->isContextValid() ? "Yes" : "No");
			ImGui::Text("Registered Plugins: %zu", uiService->getRegisteredPluginCount());

			if (ImGui::Button("Show Detailed Info")) {
				// This would typically be logged or shown in a separate window
				std::string info = uiService->getDiagnosticInfo();
				// For now, just print to console (in a real app, you'd show this in UI)
				printf("UI Service Diagnostics:\n%s\n", info.c_str());
			}
		}
	}
}

#include <imgui.h>  // Required for the inline functions