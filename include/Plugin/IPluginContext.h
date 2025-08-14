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
///		IPluginContext acts as a contract between the engine and a given plugin - what it can do, what other services it can access (ie handlers, loggers, ui etc.)
///		It is used to give limited access to resources, or other plugins
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

template <typename T>
bool IPluginContext::hasService() const
{
	using CleanType = std::remove_cvref_t<T>;
	return hasServiceByTypeIndex(std::type_index(typeid(CleanType)));
}

template <typename T>
void IPluginContext::registerTypedHandler(const std::shared_ptr<ITypedDataPacketHandler<T>>& handler)
{
	auto adapter = std::make_shared<TypedDataPacketHandlerAdapter<T>>(handler);
	registerDataPacketHandler(typeid(T), adapter);
}