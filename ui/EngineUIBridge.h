#pragma once

#include "../include/DataPacket/DataPacketRegistry.h"
#include "../include/Plugin/PluginManager.h"

class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(std::shared_ptr<PluginManager> pluginMgr,
		std::shared_ptr<DataPacketRegistry> registry,
		std::shared_ptr<ILogger> logger);

	void drawPluginUI();
	void drawDataPacketUI();
	void drawMenuBar();
	void drawSideBar() const;
	void drawMainPanels();
	void drawStatusBar();
	void drawConfigUI();
	void draw();


	bool shouldQuit() const;

private:
	std::shared_ptr<PluginManager> m_pluginManager;
	std::shared_ptr<DataPacketRegistry> m_dataPacketRegistry;
	std::shared_ptr<ILogger> m_logger;


	bool showConfigWindow = false;

	bool quitRequested = false;
};
