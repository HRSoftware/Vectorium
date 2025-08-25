#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>



class ServiceContainer
{
public:

	// Type-erased core methods
	void registerServiceByType(std::type_index typeIdx, std::shared_ptr<void> service);
	void unregisterServiceByType(std::type_index typeIdx);
	std::shared_ptr<void> getServiceByType(std::type_index typeIdx) const;
	bool hasServiceByType(std::type_index tIdx) const;
	void clear();


		// Template wrapper for type safety
	template<typename T>
	void registerService(std::shared_ptr<T> service)
	{
		registerServiceByType(std::type_index(typeid(T)), service);
	}

	template<typename T>
	std::shared_ptr<T> getService() const
	{
		auto service = getServiceByType(std::type_index(typeid(T)));
		return std::static_pointer_cast<T>(service);
	}

	template<typename T>
	bool hasService() const
	{
		return hasServiceByType(std::type_index(typeid(T)));
	}

	template<typename T>
	bool unregisterService()
	{
		auto typeIdx = std::type_index(typeid(T));
		bool existed = hasServiceByType(typeIdx);
		unregisterServiceByType(typeIdx);
		return existed;
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<void>> m_services;
};
