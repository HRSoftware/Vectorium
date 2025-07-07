#include "EngineUIBridge.h"
//#include <cxxabi.h>
#include <memory>

#include "imgui.h"
#include "../include/Engine.h"
#include "Plugin/LoadedPlugin.h"

//static std::string demangle(const char* name) {
//	int status = 0;
//	std::unique_ptr<char, void(*)(void*)> result(
//		abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free);
//	return (status == 0) ? result.get() : name;
//}



EngineUIBridge::EngineUIBridge(PluginManager& pm, DataPacketRegistry& dpr) : pluginManager(pm), dataPacketRegistry(dpr)
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
				quitRequested = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Plugins")) {
			for (const auto& info : pluginManager.getDiscoveredPlugins()) {
				if (ImGui::MenuItem(info.name.c_str(), nullptr, info.loaded)) {
					if (!info.loaded) {
						pluginManager.loadPlugin(info.path);
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
		for (const auto& pluginName : pluginManager.getNamesOfAllLoadedPlugins()) {
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
		ImGui::Text("Loaded Plugins: %zu", pluginManager.getLoadedPlugins().size());
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


