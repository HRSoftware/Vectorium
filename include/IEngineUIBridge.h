#pragma once

#include <memory>

class PluginManager;
class DataPacketRegistry;
class ILogger;
class UILogSink;
class EngineSettings;
class UIServiceManager;
class IUIService;

/// <summary>
/// Interface providing UI with only what it needs from Engine
/// Breaks circular dependency by abstracting Engine details
/// </summary>
class IEngineUIBridge
{
public:
	virtual ~IEngineUIBridge() = default;

	// UI only needs these specific things from Engine
	virtual PluginManager* getPluginManager() const = 0;
	virtual DataPacketRegistry* getDataPacketRegistry() const = 0;
	virtual std::shared_ptr<ILogger> getLogger() const = 0;
	virtual std::shared_ptr<UILogSink> getLogSink() const = 0;
	virtual EngineSettings& getEngineSettings() = 0;

	virtual void setUIService(std::shared_ptr<IUIService> uiService) = 0;
	virtual std::shared_ptr<IUIService> getUIService() const = 0;

	virtual void notifyUIInitialised() = 0;
};