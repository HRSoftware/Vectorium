#include "EngineUIBridge.h"
//#include <cxxabi.h>
#include <memory>
#include <ranges>
#include <utility>

#include "Engine.h"
#include "imgui.h"
#include "UI/ImguiContextManager.h"
#include "imgui_internal.h"
#include "Plugin/IPlugin.h"
#include "Services/Logging/UILogSink.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include "Plugin/PluginInstance.h"
#include "Plugin/PluginManager.h"
#include "Services/UI/PluginUIService_ImGui.h"
#include "UI/UIServiceManager.h"

//static std::string demangle(const char* name) {
//	int status = 0;
//	std::unique_ptr<char, void(*)(void*)> result(
//		abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
//	return (status == 0) ? result.get() : name;
//}

namespace
{
	ImVec4 getColorForLevel(spdlog::level::level_enum level)
	{
		switch (level)
		{
			case spdlog::level::trace:    return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
			case spdlog::level::debug:    return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
			case spdlog::level::info:     return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
			case spdlog::level::warn:     return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			case spdlog::level::err:      return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			case spdlog::level::critical: return ImVec4(1.0f, 0.0f, 1.0f, 1.0f);
			case spdlog::level::off:
			case spdlog::level::n_levels:
			default:                      return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

EngineUIBridge::EngineUIBridge(IEngineUIBridge& engineBridge) : m_engineBridge(engineBridge),
                                                                m_logger(*engineBridge.getLogger()),
                                                                m_showPluginUIs(false)
{
}

void EngineUIBridge::drawConfigUI()
{
	if (!showConfigWindow) return;
	ImGui::Begin("Plugin Config", &showConfigWindow);
	auto pluginManager = m_engineBridge.getPluginManager();
	if(!pluginManager)
	{
		m_logger.log(LogLevel::Error, "Could not get pluginManager");
		return;
	}
	const auto& config = pluginManager->getConfig();

	bool autoScan = config.autoScan;
	int interval = static_cast<int>(config.pluginScanInterval.count());

	if(ImGui::Checkbox("Auto-scan", &autoScan))
	{
		pluginManager->setAutoScan(autoScan);
	}

	if (ImGui::InputInt("Scan Interval (sec)", &interval))
	{
		if (interval > 0)
		{
			pluginManager->setScanInterval(std::chrono::seconds(interval));
		}
	}

	if (ImGui::Button("Save Config"))
	{
		if(pluginManager->saveConfig())
		{
			pluginManager->reloadPluginConfig();
		}
		else
		{
			m_logger.log(LogLevel::Error, "Failed to save config");
		}
	}

	ImGui::End();
}

void EngineUIBridge::drawEngineSettingsUI()
{

	if (!showEngineWindow) return;
	ImGui::Begin("Settings", &showEngineWindow);

	bool bEngineDebugLogging = m_engineBridge.getEngineSettings().isDebugLoggingEnabled();

	if(ImGui::Checkbox("Engine debug logging", &bEngineDebugLogging))
	{
		m_engineBridge.getEngineSettings().setDebugLogging(bEngineDebugLogging);
	}

	ImGui::End();
}

void EngineUIBridge::drawPluginUI() const
{
	if (!ImGui::Begin("Plugin UI"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Plugin Management");
	ImGui::Separator();

	std::vector<std::string> pluginsToUnload;  // Collect names (we crash if we unload on a currently displaying plugin)

	for (const auto& [name, plugin] : m_engineBridge.getPluginManager()->getLoadedPlugins())
	{
		if (ImGui::CollapsingHeader(name.c_str()))
		{
			ImGui::Indent();

			ImGui::Text("Status: Running");
			ImGui::Text("Debug: %s", plugin->isPluginDebugLoggingEnabled() ? "On" : "Off");

			if (ImGui::Button(("Unload##" + name).c_str()))
			{
				pluginsToUnload.push_back(name);
			}

			ImGui::SameLine();
			if (ImGui::Button(("Toggle Debug##" + name).c_str()))
			{
				plugin->isPluginDebugLoggingEnabled()
				? plugin->disablePluginDebugLogging()
				: plugin->enablePluginDebugLogging();
			}

			ImGui::Unindent();
		}
	}

	ImGui::End();

	for(const auto& pluginName : pluginsToUnload)
	{
		m_engineBridge.getPluginManager()->unloadPlugin(pluginName);
	}
}

void EngineUIBridge::drawDataPacketUI()
{
}

void EngineUIBridge::drawMenuBar()
{
	auto pluginManager = m_engineBridge.getPluginManager();
	if(!pluginManager)
	{
		m_logger.log(LogLevel::Error, "Could not get PluginManger");
		return;
	}
	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Plugin Dir watcher", nullptr, pluginManager->isPluginFolderWatcherEnabled()))
			{
				pluginManager->isPluginFolderWatcherEnabled() ? pluginManager->stopPluginAutoScan() : pluginManager->startPluginAutoScan();
			}

			ImGui::MenuItem("Plugin Config", nullptr, &showConfigWindow);

			ImGui::MenuItem("Settings", nullptr, &showEngineWindow);

			ImGui::MenuItem("Log Panel", nullptr, &showLogPanel);

			if (ImGui::MenuItem("Quit"))
			{
				m_logger.log(LogLevel::Info, "Quit requested");
				quitRequested = true;
			}
			ImGui::EndMenu();
		}



		if (ImGui::BeginMenu("Plugins"))
		{
			for (const auto& [name, pluginInfo] : pluginManager->getDiscoveredPlugins())
			{
				if (ImGui::MenuItem(name.c_str(), nullptr, pluginInfo.loaded))
				{
					pluginInfo.loaded ? pluginManager->unloadPlugin(name) : pluginManager->loadPlugin(pluginInfo.path);
				}
			}

			ImGui::Separator();

			ImGui::MenuItem("Plugin Debug Logging options", nullptr, &showLoggingSettingsWindow);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows"))
		{
			// Show available plugin windows
			for (const auto& pluginInstance : pluginManager->getLoadedPlugins() | std::views::values)
			{
				if(auto* plugin = pluginInstance->getPlugin())
				{
					if (plugin->hasUIWindow())
					{
						const bool isVisible = plugin->isUIWindowVisible();
						if (ImGui::MenuItem(plugin->getUIWindowTitle().c_str(), nullptr, isVisible))
						{
							plugin->toggleUIWindow();
						}
					}
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EngineUIBridge::drawSideBar() const
{
	auto pluginManager = m_engineBridge.getPluginManager();
	if(!pluginManager)
	{
		m_logger.log(LogLevel::Error, "Could not get PluginManger");
		return;
	}

	// Get the main viewport to calculate positioning
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Sidebar configuration
	constexpr float sidebarWidth = 250.0f;  // Adjust width as needed
    const float menuBarHeight = ImGui::GetFrameHeight(); // Account for menu bar

    // Position sidebar on the right side
    ImVec2 sidebarPos = ImVec2(
        viewport->Pos.x + viewport->Size.x - sidebarWidth,  // Right edge
        viewport->Pos.y + menuBarHeight                     // Below menu bar
    );

    ImVec2 sidebarSize = ImVec2(
        sidebarWidth,
        viewport->Size.y - menuBarHeight  // Full height minus menu bar
    );

    // Set window flags to pin it in place
    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoMove |           // Can't be moved
        ImGuiWindowFlags_NoResize |         // Can't be resized
        ImGuiWindowFlags_NoCollapse |       // Can't be collapsed
        ImGuiWindowFlags_NoTitleBar |       // No title bar for cleaner look
        ImGuiWindowFlags_NoBringToFrontOnFocus; // Don't steal focus

    // Set next window position and size
    ImGui::SetNextWindowPos(sidebarPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(sidebarSize, ImGuiCond_Always);

    // Create the pinned sidebar
    if (ImGui::Begin("##PluginSidebar", nullptr, windowFlags))
    {
        // Sidebar header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
        ImGui::Text("LOADED PLUGINS");
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::Spacing();

        // Plugin list
        const auto& loadedPlugins = pluginManager->getLoadedPlugins();

        if (loadedPlugins.empty())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::Text("No plugins loaded");
            ImGui::PopStyleColor();
        }
        else
        {
            for (const auto& [pluginName, plugin] : loadedPlugins)
            {
                // Plugin entry with status indicator
                ImGui::PushID(pluginName.c_str());

                // Status indicator (green dot for running)
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("X");
                ImGui::PopStyleColor();

                ImGui::SameLine();

                // Plugin name
                if (ImGui::Selectable(pluginName.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        // Double-click to toggle debug logging
                        if (plugin->isPluginDebugLoggingEnabled())
						{
                            plugin->disablePluginDebugLogging();
                        } else {
                            plugin->enablePluginDebugLogging();
                        }
                    }
                }

                // Context menu for plugin actions
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Unload Plugin"))
                    {
                        // Store name for later unloading (avoid iterator invalidation)
                        m_pluginToUnload = pluginName;
                    }

                    if (ImGui::MenuItem("Toggle Debug Logging"))
                    {
                        if (plugin->isPluginDebugLoggingEnabled())
						{
                            plugin->disablePluginDebugLogging();
                        } else {
                            plugin->enablePluginDebugLogging();
                        }
                    }

                    ImGui::EndPopup();
                }

            	// Show debug status
                if (plugin->isPluginDebugLoggingEnabled())
                {
                    ImGui::SameLine();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 0.7f));
                    ImGui::Text("(DEBUG)");
                    ImGui::PopStyleColor();
                }

            	ImGui::PopID();
            }
        }

    	ImGui::Spacing();
        ImGui::Separator();

    	// Quick stats
        ImGui::Text("Total: %zu", loadedPlugins.size());

    	// Quick actions
        if (ImGui::Button("Reload All"))
        {
            // Implement reload all functionality
            m_shouldReloadAllPlugins = true;
        }
    }
    ImGui::End();

	// Handle deferred actions (to avoid iterator invalidation)
    if (!m_pluginToUnload.empty())
    {
        pluginManager->unloadPlugin(m_pluginToUnload);
        m_pluginToUnload.clear();
    }

	if (m_shouldReloadAllPlugins)
    {
        // Implement reload all logic
        m_shouldReloadAllPlugins = false;
    }
}

void EngineUIBridge::drawMainPanels() const
{
	
}

void EngineUIBridge::drawStatusBar()
{
	auto pluginManager = m_engineBridge.getPluginManager();
	if(!pluginManager)
	{
		m_logger.log(LogLevel::Error, "Could not get PluginManger");
		return;
	}

	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::Text("Loaded Plugins: %zu", pluginManager->getLoadedPlugins().size());
	ImGui::End();
}

void EngineUIBridge::drawLoggingSettingUI()
{
	auto pluginManager = m_engineBridge.getPluginManager();
	if(!pluginManager)
	{
		m_logger.log(LogLevel::Error, "Could not get PluginManger");
		return;
	}
	if (!showLoggingSettingsWindow) return;
	ImGui::Begin("Plugin Config", &showLoggingSettingsWindow);

	bool isDebugEnabled;
	for(const auto& [name, plugin] : pluginManager->getLoadedPlugins())
	{
		isDebugEnabled = plugin->isPluginDebugLoggingEnabled();
		if(ImGui::Checkbox(name.c_str(), &isDebugEnabled))
		{
			isDebugEnabled ? plugin->enablePluginDebugLogging() : plugin->disablePluginDebugLogging();
		}
	}

	ImGui::End();
}

void EngineUIBridge::drawLogPanel(UILogSink& uiSink)
{
	if (!showLogPanel) return;

	if(ImGui::Begin("Log Console", &showLogPanel))
	{
		// Control buttons
		if(ImGui::Button("Clear"))
		{
			uiSink.clearBuffer();
		}
		ImGui::SameLine();

		static bool autoScroll = true;
		ImGui::Checkbox("Auto-scroll", &autoScroll);

		ImGui::Separator();

		// Log content in scrollable region
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

		for(const auto& entry : uiSink.logBuffer)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, getColorForLevel(entry.level));
			ImGui::TextUnformatted(entry.message.c_str());
			ImGui::PopStyleColor();
		}

		// Auto-scroll to bottom if enabled
		if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void EngineUIBridge::draw()
{
	drawMenuBar();
	//ImGui::DockSpaceOverViewPort();

	drawSideBar();
	drawMainPanels();
	drawStatusBar();

	drawConfigUI();
	drawLoggingSettingUI();
	drawEngineSettingsUI();
	drawLogPanel(*m_engineBridge.getLogSink());

	if(m_showPluginUIs)
	{
		//drawPluginUI();
	}
}



bool EngineUIBridge::shouldQuit() const
{
	return quitRequested;
}


