
#include "../include/Core/PluginContextImpl.h"

#include <iostream>
#include <format>

void PluginContextImpl::registerHandler(const std::type_index type, std::function<std::unique_ptr<IDataHandler>()> factory)
{
	pluginRegister[type] = std::move(factory);
	registeredTypes.insert(type);
	std::cout << std::format("Registered handle for {}\n", type.name());

}

void PluginContextImpl::unregisterHandler(const std::type_index type)
{
	pluginRegister.erase(type);
	registeredTypes.erase(type);
	std::cout << std::format("Unregistered handle for {}\n", type.name());
}
