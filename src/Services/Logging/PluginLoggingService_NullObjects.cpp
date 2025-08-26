#include "Services/IServiceSpecialisations.h"
#include "Services/IService.h"
#include "Services/Logging/ILogger.h"
#include "Services/Logging/SpdLogger.h"



template class NullService<ILogger>;
template class ServiceProxy<ILogger>;

template class NullService<SpdLogger>;
template class ServiceProxy<SpdLogger>;