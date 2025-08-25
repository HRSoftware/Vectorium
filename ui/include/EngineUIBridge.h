#pragma once

#include "Services/Logging/UILogSink.h"
#include "DataPacket/DataPacketRegistry.h"
#include "Plugin/PluginManager.h"

class IEngineUIBridge;
class EngineSettings;
class Engine;

class EngineUIBridge
{
public:
	EngineUIBridge(IEngineUIBridge& engineBridge);

	void drawPluginUI() const;
	void drawMenuBar();
	void drawSideBar() const;
	void drawConfigUI();
	void drawEngineSettingsUI();
	void drawMainPanels() const;
	void drawStatusBar();

	void drawDataPacketUI();
	void drawLoggingSettingUI();

	void drawLogPanel(UILogSink& uiSink);

	void draw();

	bool shouldQuit() const;

private:

	IEngineUIBridge& m_engineBridge;  // Use interface reference
	ILogger& m_logger;

	bool showConfigWindow = false;
	bool showEngineWindow = false;
	bool showLoggingSettingsWindow = false;
	bool showLogPanel = false;

	bool quitRequested = false;
	bool  m_showPluginUIs;
};
