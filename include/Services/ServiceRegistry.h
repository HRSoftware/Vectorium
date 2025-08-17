#pragma once
#include <variant>
#include "IService.h"
#include "ServiceId.h"

/// <summary>
/// Manages registration and retrieval of service instances by type.
/// </summary>
class ServiceRegistry
{
public:
	/// <summary>
	/// Registers a service instance of the specified type.
	/// </summary>
	/// <typeparam name="T">The type of the service to register.</typeparam>
	/// <param name="service">A shared pointer to the service instance to register.</param>
	template<typename T>
	void registerService(std::shared_ptr<T> service)
	{
		m_services[typeid(T)] = service;
	}

	/// <summary>
	/// Retrieves a shared pointer to a service of the specified type.
	/// </summary>
	/// <typeparam name="T">The type of the service to retrieve.</typeparam>
	/// <returns>A shared pointer to the requested service if found; otherwise, nullptr.</returns>
	template<typename T>
	std::shared_ptr<T> getService() const
	{
		auto it = m_services.find(typeid(T));
		if (it != m_services.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}

		return nullptr;
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<void>> m_services;
};

