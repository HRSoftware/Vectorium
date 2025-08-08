#pragma once

#include "UILogSink.h"
#include "../include/DataPacket/DataPacketRegistry.h"
#include "../include/Plugin/PluginManager.h"

class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(std::shared_ptr<PluginManager> pluginMgr,
		std::shared_ptr<DataPacketRegistry> registry,
		std::shared_ptr<ILogger> logger,
		std::shared_ptr<UILogSink> logSink);

	void drawPluginUI();
	void drawDataPacketUI();
	void drawMenuBar();
	void drawSideBar() const;
	void drawMainPanels();
	void drawStatusBar();
	void drawLoggingSettingUI();
	void drawLogPanel(std::shared_ptr<UILogSink> uiSink);
	void drawConfigUI();
	void draw();


	bool shouldQuit() const;

private:
	std::shared_ptr<PluginManager> m_pluginManager;
	std::shared_ptr<DataPacketRegistry> m_dataPacketRegistry;
	std::shared_ptr<ILogger> m_logger;
	std::shared_ptr<UILogSink> m_LogSink;


	bool showConfigWindow = false;
	bool showLoggingSettingsWindow = false;
	bool showLogPanel = false;

	bool quitRequested = false;
};
