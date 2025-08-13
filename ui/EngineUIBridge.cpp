#include "EngineUIBridge.h"
//#include <cxxabi.h>
#include <memory>
#include <utility>

#include "Engine.h"
#include "imgui.h"
#include "UILogSink.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/LogLevel.h"
#include "Plugin/PluginInstance.h"
#include "Plugin/PluginManager.h"

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

EngineUIBridge::EngineUIBridge(
	std::shared_ptr<PluginManager> pluginMgr,
	std::shared_ptr<DataPacketRegistry> registry,
	std::shared_ptr<ILogger> logger,
	std::shared_ptr<UILogSink> logSink)
: m_pluginManager(std::move(pluginMgr))
, m_dataPacketRegistry(std::move(registry))
, m_logger(std::move(logger))
, m_LogSink(std::move(logSink))
{
}

void EngineUIBridge::drawConfigUI()
{
	if (!showConfigWindow) return;
	ImGui::Begin("Plugin Config", &showConfigWindow);

	const auto& config = m_pluginManager->getConfig();
	bool autoScan = config.autoScan;
	int interval = static_cast<int>(config.pluginScanInterval.count());

	if(ImGui::Checkbox("Auto-scan", &autoScan))
	{
		m_pluginManager->setAutoScan(autoScan);
	}

	if (ImGui::InputInt("Scan Interval (sec)", &interval))
	{
		if (interval > 0)
		{
			m_pluginManager->setScanInterval(std::chrono::seconds(interval));
		}
	}

	if (ImGui::Button("Save Config"))
	{
		if(m_pluginManager->saveConfig())
		{
			m_pluginManager->reloadPluginConfig();
		}
		else
		{
			m_logger->log(LogLevel::Error, "Failed to save config");
		}
	}

	ImGui::End();
}

void EngineUIBridge::drawPluginUI()
{
}

void EngineUIBridge::drawDataPacketUI()
{
}

void EngineUIBridge::drawMenuBar()
{
	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Plugin Dir watcher", nullptr, m_pluginManager->isPluginFolderWatcherEnabled()))
			{
				m_pluginManager->isPluginFolderWatcherEnabled() ? m_pluginManager->stopPluginAutoScan() : m_pluginManager->startPluginAutoScan();
			}

			ImGui::MenuItem("Plugin Config", nullptr, &showConfigWindow);

			ImGui::MenuItem("Log Panel", nullptr, &showLogPanel);

			if (ImGui::MenuItem("Quit"))
			{
				m_logger->log(LogLevel::Info, "Quit requested");
				quitRequested = true;
			}
			ImGui::EndMenu();
		}



		if (ImGui::BeginMenu("Plugins"))
		{
			for (const auto& [name, pluginInfo] : m_pluginManager->getDiscoveredPlugins())
			{
				if (ImGui::MenuItem(name.c_str(), nullptr, pluginInfo.loaded))
				{
					pluginInfo.loaded ? m_pluginManager->unloadPlugin(name) : m_pluginManager->loadPlugin(pluginInfo.path);
				}
			}

			ImGui::Separator();

			ImGui::MenuItem("Plugin Debug Logging options", nullptr, &showLoggingSettingsWindow);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EngineUIBridge::drawSideBar() const
{
	ImGui::Begin("Sidebar");
		ImGui::Text("Loaded Plugins:");
		for (const auto& pluginName : m_pluginManager->getNamesOfAllLoadedPlugins()) {
			ImGui::BulletText("%s", pluginName.c_str());
		}
	ImGui::End();
}

void EngineUIBridge::drawMainPanels()
{
}

void EngineUIBridge::drawStatusBar()
{
	ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::SameLine();
		ImGui::Text("Loaded Plugins: %zu", m_pluginManager->getLoadedPlugins().size());
	ImGui::End();
}

void EngineUIBridge::drawLoggingSettingUI()
{
	if (!showLoggingSettingsWindow) return;
	ImGui::Begin("Plugin Config", &showLoggingSettingsWindow);

	bool isDebugEnabled;
	for(const auto& [name, plugin] : m_pluginManager->getLoadedPlugins())
	{
		isDebugEnabled = plugin->isDebugLoggingEnabled();
		if(ImGui::Checkbox(name.c_str(), &isDebugEnabled))
		{
			isDebugEnabled ? plugin->enableDebugLogging() : plugin->disableDebugLogging();
		}
	}

	ImGui::End();
}

void EngineUIBridge::drawLogPanel(std::shared_ptr<UILogSink> uiSink)
{
	if (!showLogPanel || !uiSink) return;

	if(ImGui::Begin("Log Console"))
	{
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
		if(ImGui::Button("Clear"))
		{
			uiSink->logBuffer.clear();
		}

		for(const auto& line : uiSink->logBuffer)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, getColorForLevel(line.level));
			ImGui::TextUnformatted(line.message.c_str());
			ImGui::PopStyleColor();
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
	drawLogPanel(m_LogSink);
}

bool EngineUIBridge::shouldQuit() const
{
	return quitRequested;
}


