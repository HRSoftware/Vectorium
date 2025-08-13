#pragma once
#include <typeindex>
#include <string>
#include <memory>
#include <type_traits>



// Forward declare for the trait
template<typename T>
struct NullObjectImpl;

template<typename T, typename = void>
struct has_null_impl : std::false_type {};

template<typename T>
struct has_null_impl<T, std::void_t<decltype(std::declval<NullObjectImpl<T>>())>> : std::true_type {};


struct ServiceId
{
	std::type_index type;
	std::string     name;      // stable string id
	std::string     minVersion; // semver-ish, e.g., ">=1.2.0"
	bool            required{true};
};


template<typename T>
class IServiceWrapper
{
public:
	virtual ~IServiceWrapper() = default;
	virtual T* operator->() = 0;
	virtual T& operator*() = 0;
	virtual bool isAvailable() const = 0;
};



// This enabled services to safely be missing, and not require constant nullptr checks
template<typename T>
class RefService : public IServiceWrapper<T>
{
	std::shared_ptr<T> m_service;
public:
	explicit RefService(std::shared_ptr<T> service) : m_service(service)
	{}

	T* operator->() override
	{
		return m_service.get();
	}

	T& operator*() override
	{
		return *m_service;
	}

	bool isAvailable() const override
	{
		return m_service != nullptr;
	}
};

template<typename T>
class NullService : public IServiceWrapper<T>
{

	static_assert(has_null_impl<T>::value, "NullObjectImpl<T> specialization required for this service type");
public:
	// Returns null object that implements T's interface with no-ops
	T* operator->() override
	{
		return &getNullImpl();
	}

	T& operator*() override
	{
		return getNullImpl();
	}

	bool isAvailable() const override
	{
		return false;
	}

private:
	static T& getNullImpl()
	{
		static NullObjectImpl<T> instance;
		return instance;
	}
};

template<typename T>
class ServiceProxy
{
	std::unique_ptr<IServiceWrapper<T>> wrapper_;

public:
	ServiceProxy(std::unique_ptr<IServiceWrapper<T>> wrapper)
		: wrapper_(std::move(wrapper)) {}

	// Forward all calls to the wrapper
	T* operator->()
	{
		return wrapper_->operator->();
	}

	T& operator*()
	{
		return wrapper_->operator*();
	}

	bool isAvailable() const
	{
		return wrapper_->isAvailable();
	}

	// Move semantics
	ServiceProxy(ServiceProxy&&) = default;
	ServiceProxy& operator=(ServiceProxy&&) = default;
};