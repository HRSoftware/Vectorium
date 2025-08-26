#pragma once
#include "Services/REST/IPluginRESTService.h"

class IPluginRESTServiceImpl : public IPluginRESTService
{
public:
	virtual ~IPluginRESTServiceImpl() = default;
	// Pure interface - no implementation details
};