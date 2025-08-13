#pragma once
#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "DataPacket/IDataPacketHandler.h"

// forward declare
class ILogger;
struct DataPacket;

/// <summary>
/// DataPacketRegistry knows about all packet types, and the handlers subscribed to each
/// </summary>
class DataPacketRegistry
{

	using pluginNameAndHandlerPointer = std::pair<std::shared_ptr<IDataPacketHandler>, std::string>;
public:
		explicit DataPacketRegistry(std::shared_ptr<ILogger> log);

		bool registerDataPacketHandler(std::type_index packetType, std::shared_ptr<IDataPacketHandler> handler, const std::string& pluginName);
		bool registerWildcardHandler(std::shared_ptr<IDataPacketHandler> handler, const std::string& pluginName);
		void unregisterDataPacketHandlerForPlugin(std::string_view pluginName);
		void dispatch(const DataPacket& packet);

		[[nodiscard]] std::vector<std::type_index> getDataPacketTypes() const;

private:
		std::unordered_map<std::type_index, std::vector<pluginNameAndHandlerPointer>> m_handlers;
		std::vector<pluginNameAndHandlerPointer> m_wildcardHandlers;

		std::shared_ptr<ILogger> logger;

		template<typename T>
		void registerTypedHandler(std::shared_ptr<ITypedDataPacketHandler<T>> typedHandler)
		{
			auto adapter = std::make_shared<TypedDataPacketHandlerAdapter<T>>(std::move(typedHandler));
			registerDataPacketHandler(adapter);
		}
};


