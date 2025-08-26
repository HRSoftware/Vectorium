#include "Services/REST/PluginRESTServiceFactory.h"

#ifdef USE_HTTPLIB
#include "Services/REST/PluginRESTService_HttpLib.h"
#endif

#ifdef USE_CPR
#include "Services/REST/PluginRESTService_Cpr.h"
#endif

std::unique_ptr<IPluginRESTService> PluginRESTServiceFactory::create(const std::string& baseUrl)
{
#ifdef USE_HTTPLIB
	return std::make_unique<PluginRESTService_HttpLib>(baseUrl);
#elif defined(USE_CPR)
	return std::make_unique<PluginRESTService_Cpr>(baseUrl);
#else
#error "No REST client implementation selected"
#endif
}