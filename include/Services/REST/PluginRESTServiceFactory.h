#pragma once
#include <memory>
#include <string>
#include <Services/REST/IPluginRESTService.h>

namespace PluginRESTServiceFactory
{
	std::unique_ptr<IPluginRESTService> create(const std::string& baseUrl);
};