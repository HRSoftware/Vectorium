#pragma once

// Redundant for now - maybe for a future, more complex policy system(if needed)

//#include "IPlugin.h"
//
//class PluginContextPolicy
//{
//public:
//	enum class ContextType
//	{
//		Sandboxed,   // Limited Services
//		Trusted,     // Full Services
//		Development, // Debug Services + extra logging
//		Custom
//	};
//
//	ContextType determineContextType(const PluginDescriptor& desc) const;
//
//private:
//
//	bool isTrustedPlugin(const std::string& name) const
//	{
//		//Need a more secure system than just using the name - but that for waaaay down the road
//		static const std::set<std::string> trustedPlugins =
//		{
//			"GPS", "WeatherService", "CoreTelemetry"
//		};
//		return trustedPlugins.contains(name);
//	}
//
//};