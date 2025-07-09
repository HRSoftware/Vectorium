#include "EngineUIBridge.h"
//#include <cxxabi.h>
#include <memory>
#include <utility>

#include "imgui.h"
#include "../include/Engine.h"
#include "Plugin/PluginInstance.h"

//static std::string demangle(const char* name) {
//	int status = 0;
//	std::unique_ptr<char, void(*)(void*)> result(
//		abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
//	return (status == 0) ? result.get() : name;
//}



EngineUIBridge::EngineUIBridge(
	std::shared_ptr<PluginManager> pluginMgr,
	std::shared_ptr<DataPacketRegistry> registry,
	std::shared_ptr<ILogger> logger)
: m_pluginManager(std::move(pluginMgr))
, m_dataPacketRegistry(std::move(registry))
, m_logger(std::move(logger))
{
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
					if (!pluginInfo.loaded)
					{
						m_pluginManager->loadPlugin(pluginInfo.path);
					}
					else
					{
						m_pluginManager->unloadPlugin(name);
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

void EngineUIBridge::draw()
{
	drawMenuBar();
	//ImGui::DockSpaceOverViewPort();

	drawSideBar();
	drawMainPanels();
	drawStatusBar();
}

bool EngineUIBridge::shouldQuit() const
{
	return quitRequested;
}


