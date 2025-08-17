#pragma once

#include "Services/Logging/UILogSink.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Plugin/PluginManager.h"

class EngineSettings;
class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(PluginManager& pluginMgr,
		DataPacketRegistry& registry,
		ILogger& logger,
		UILogSink& logSink,
		EngineSettings& engineSettings);

	void drawPluginUI();
	void drawMenuBar();
	void drawSideBar() const;
	void drawConfigUI();
	void drawEngineSettingsUI();
	void drawMainPanels();
	void drawStatusBar();

	void drawDataPacketUI();
	void drawLoggingSettingUI();

	void drawLogPanel(UILogSink& uiSink);
	
	void draw();


	bool shouldQuit() const;

private:
	PluginManager& m_pluginManager;
	DataPacketRegistry& m_dataPacketRegistry;
	ILogger& m_logger;
	UILogSink& m_LogSink;
	EngineSettings& m_engineSettings;


	bool showConfigWindow = false;
	bool showEngineWindow = false;
	bool showLoggingSettingsWindow = false;
	bool showLogPanel = false;

	bool quitRequested = false;
};
