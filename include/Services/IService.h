#pragma once
#include <typeindex>
#include <string>
#include <memory>
#include <type_traits>


template<typename T>
struct NullObjectImpl;

template<typename T, typename = void>
struct has_null_impl : std::false_type {};

template<typename T>
struct has_null_impl<T, std::void_t<decltype(std::declval<NullObjectImpl<T>>())>> : std::true_type {};


template<typename T>
class IServiceWrapper
{
public:
	virtual ~IServiceWrapper() = default;
	virtual T* operator->() = 0;
	virtual T& operator*() = 0;
	virtual bool isAvailable() const = 0;
};



/// <summary>
/// A wrapper class that provides access to a service object via a shared pointer, implementing IServiceWrapper<T>.
/// </summary>
/// <remarks>This enabled services to safely be missing, and not require constant nullptr checks</remarks>
/// <typeparam name="T">The type of the service object.</typeparam>
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

/// <summary>
/// A proxy class that manages access to a service implementation via an IServiceWrapper. Provides pointer-like semantics and availability checking.
/// </summary>
/// <typeparam name="T">The type of the service interface being proxied.</typeparam>
template<typename T>
class ServiceProxy
{
	std::unique_ptr<IServiceWrapper<T>> wrapper_;

public:
	ServiceProxy() : wrapper_(std::make_unique <NullService<T>>())
	{
		if constexpr (has_null_impl<T>::value)
		{
			wrapper_ = std::make_unique<NullService<T>>();
		}
		else
		{
			wrapper_ = std::make_unique<RefService<T>>(nullptr);
		}
	}

	explicit ServiceProxy(std::unique_ptr<IServiceWrapper<T>> wrapper)
		: wrapper_(std::move(wrapper)) {}

	// Forward all calls to the wrapper
	T* operator->()
	{
		if(!wrapper_)
		{
			if constexpr (has_null_impl<T>::value)
			{
				wrapper_ = std::make_unique<NullService<T>>();
			}
			else
			{
				// Return nullptr and let it crash with a more obvious error
				return nullptr;
			}
		}

		return wrapper_->operator->();
	}

	T& operator*()
	{
		if (!wrapper_)
		{
			wrapper_ = std::make_unique<NullService<T>>();
		}
		return wrapper_->operator*();
	}

	bool isAvailable() const
	{
		if (!wrapper_)
		{
			return false;  // Safe return instead of crash
		}
		return wrapper_->isAvailable();
	}

	// Move semantics
	ServiceProxy(ServiceProxy&&) = default;
	ServiceProxy& operator=(ServiceProxy&&) = default;
};