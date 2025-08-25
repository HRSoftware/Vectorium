#include "Services/ServiceContainer.h"

#include <iostream>

void ServiceContainer::registerServiceByType(std::type_index typeIdx, std::shared_ptr<void> service)
{
	if(!service)
	{
		std::cout << std::format("WARNING: Attempting to register null service {}\n", typeIdx.name());
		return;
	}

	m_services[typeIdx] = service;
}

void ServiceContainer::unregisterServiceByType(std::type_index typeIdx)
{
	if (m_services.erase(typeIdx) > 0)
	{
		std::cout << std::format("ServiceContainer: Unregistered service {}\n", typeIdx.name());
	}
}

std::shared_ptr<void> ServiceContainer::getServiceByType(std::type_index typeIdx) const
{
	if (const auto it = m_services.find(typeIdx); it != m_services.end())
	{
		return it->second;
	}

	return nullptr;
}

void ServiceContainer::clear()
{
	m_services.clear();
}

bool ServiceContainer::hasServiceByType(std::type_index tIdx) const
{
	return m_services.contains(tIdx);
}
