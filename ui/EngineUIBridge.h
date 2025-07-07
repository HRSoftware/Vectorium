#pragma once

#include "../include/DataPacket/DataPacketRegistry.h"
#include "../include/Plugin/PluginManager.h"

class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(PluginManager& pm, DataPacketRegistry& dpr);

	void drawPluginUI();
	void drawDataPacketUI();
	void drawMenuBar();
	void drawSideBar() const;
	void drawMainPanels();
	void drawStatusBar();
	void draw();


	bool shouldQuit() const;

private:
	PluginManager& pluginManager;
	DataPacketRegistry& dataPacketRegistry;

	bool quitRequested = false;
};
