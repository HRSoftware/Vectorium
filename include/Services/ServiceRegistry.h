#pragma once
#include <variant>
#include "IService.h"
#include "ServiceId.h"

class ServiceRegistry
{
public:
	template<class T>
	std::variant<RefService<T>, NullService<T>> resolve();

	bool satisfies(const ServiceId& need) const;
};

