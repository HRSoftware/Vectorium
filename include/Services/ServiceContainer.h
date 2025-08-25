#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>

class ServiceContainer
{
public:
	template<typename T>
	void registerService(std::shared_ptr<T> service)
	{
		m_services[std::type_index(typeid(T))] = service;
	}

	template<typename T>
	std::shared_ptr<T> getService() const
	{
		auto service = getService(std::type_index(typeid(T)));
		return std::static_pointer_cast<T>(service);
	}

	std::shared_ptr<void> getService(std::type_index tIdx) const
	{
		const auto it = m_services.find(tIdx);
		return (it != m_services.end()) ? it->second : nullptr;
	}

	bool hasService(std::type_index tIdx) const
	{
		return m_services.contains(tIdx);
	}

	template<typename T>
	bool hasService() const
	{
		return m_services.contains(std::type_index(typeid(T)));
	}

	template<typename T>
	void removeService()
	{
		m_services.erase(std::type_index(typeid(T)));
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<void>> m_services;
};