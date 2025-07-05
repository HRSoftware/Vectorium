#include "EngineUIBridge.h"
//#include <cxxabi.h>
#include <memory>

#include "../include/Engine.h"

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
{}

void EngineUIBridge::drawDataPacketUI()
{
}

