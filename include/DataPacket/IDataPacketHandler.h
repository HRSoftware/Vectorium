#pragma once
#include "DataPacket.h"

/// <summary>
///		IDataPacketHandler is how the DataPacket is processes
/// </summary>
class IDataPacketHandler
{
public:
	virtual bool handle(const DataPacket& packet) = 0;
	virtual ~IDataPacketHandler() = default;
};

template<typename T>
class ITypedDataPacketHandler
{
public:
	virtual bool handleType(const std::shared_ptr<T>& data) = 0;
	virtual ~ITypedDataPacketHandler() = default;
};

template<typename T>
class TypedDataPacketHandlerAdapter : public IDataPacketHandler
{
	public:
		explicit TypedDataPacketHandlerAdapter(std::shared_ptr<ITypedDataPacketHandler<T>> typeHandler) : m_typedHandler(std::move(typeHandler))
		{}

		bool handle(const DataPacket& packet) override
		{
			auto result = packet.get<T>();
			if (!result.has_value()) return false;

			return m_typedHandler->handleType(result.value());
		}

	private:
		std::shared_ptr<ITypedDataPacketHandler<T>> m_typedHandler;
};