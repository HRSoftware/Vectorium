#pragma once

#include <format>

#include "DataPacket/DataPacket.h"
#include "DataPacket/IDataPacketHandler.h"

#include "Services/IService.h"


class ILogger;
enum class LogLevel;
class IRestClient;
class IDataPacketHandler;

/// <summary>
/// Defines the interface for a plugin context, providing access to services, data packet handling, logging, and plugin metadata.
/// </summary>
class IPluginContext
{
public:
	virtual ~IPluginContext() = default;

	// Modern service access with ServiceWrapper pattern
	template<typename T>
	std::unique_ptr<IServiceWrapper<T>> getService();

	template<typename T>
	bool hasService() const;

	// Data packet handling
	template<typename T>
	void registerTypedHandler(const std::shared_ptr<ITypedDataPacketHandler<T>>& handler);

	virtual void dispatch(const DataPacket& packet) = 0;

	//Redundant?
	// Logging interface
	virtual void log(LogLevel level, const std::string& message) const = 0;


	virtual std::shared_ptr<ILogger> getLoggerShared() = 0;

	// Plugin metadata
	virtual std::string getPluginName() const = 0;

	// Legacy service access (deprecated - use getService<T>() instead)
	template<class T>
	[[deprecated("Use getService<T>() instead")]]
	std::shared_ptr<T> get()
		{
		auto service = getService<T>();
		if (service->isAvailable()) {
			return std::shared_ptr<T>(&(*service), [](T*) {}); // Non-owning shared_ptr
		}
		return nullptr;
	}

protected:
	virtual bool registerDataPacketHandler(std::type_index type,
		std::shared_ptr<IDataPacketHandler> handler) = 0;

private:
	// Type-erased virtual methods for service access
	virtual std::expected<std::shared_ptr<void>, std::string> getServiceByTypeIndex(std::type_index tIdx) = 0;
	virtual bool hasServiceByTypeIndex(std::type_index tIdx) const = 0;
};


/// <summary>
/// Retrieves a service wrapper for the specified type from the plugin context.
/// </summary>
/// <typeparam name="T">The type of the service to retrieve.</typeparam>
/// <returns>A unique pointer to an IServiceWrapper for the requested service type. If the service is available, returns a RefService wrapping the service instance. If the service is unavailable but a NullObjectImpl specialization exists, returns a NullService. Otherwise, returns a RefService with a nullptr.</returns>
template <typename T>
std::unique_ptr<IServiceWrapper<T>> IPluginContext::getService()
{
	using CleanType = std::remove_cvref_t<T>;
	auto result     = getServiceByTypeIndex(std::type_index(typeid(CleanType)));

	if (result.has_value())
	{
		auto service = std::static_pointer_cast<CleanType>(result.value());
		return std::make_unique<RefService<CleanType>>(std::move(service));
	}

	// Only create NullService for types that have NullObjectImpl specializations
	if constexpr (has_null_impl<CleanType>::value)
	{
		return std::make_unique<NullService<CleanType>>();
	}
	else
	{
		// Return RefService with nullptr (will show as unavailable)
		return std::make_unique<RefService<CleanType>>(nullptr);
	}
}

/// <summary>
/// Checks if a service of the specified type is available in the plugin context.
/// </summary>
/// <typeparam name="T">The type of the service to check for.</typeparam>
/// <returns>True if a service of the specified type exists; otherwise, false.</returns>
template <typename T>
bool IPluginContext::hasService() const
{
	using CleanType = std::remove_cvref_t<T>;
	return hasServiceByTypeIndex(std::type_index(typeid(CleanType)));
}

/// <summary>
/// Registers a typed data packet handler for a specific data type in the plugin context.
/// </summary>
/// <typeparam name="T">The type of data packets that the handler processes.</typeparam>
/// <param name="handler">A shared pointer to the typed data packet handler to be registered.</param>
template <typename T>
void IPluginContext::registerTypedHandler(const std::shared_ptr<ITypedDataPacketHandler<T>>& handler)
{
	auto adapter = std::make_shared<TypedDataPacketHandlerAdapter<T>>(handler);
	registerDataPacketHandler(typeid(T), adapter);
}