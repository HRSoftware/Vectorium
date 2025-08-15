#pragma once

#include <chrono>
#include <expected>
#include <memory>
#include <string>
#include <typeindex>


/// <summary>
/// DataPacket is the atom form of a type of data
/// </summary>
struct DataPacket
{
	std::shared_ptr<void> payload;
	std::type_index payloadType;
	std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
	//some form of ID?

	template<typename T>
	static DataPacket create(std::shared_ptr<T> data)
	{
		return DataPacket{
			.payload = std::move(data),
			.payloadType = typeid(T)
		};
	}

	template<typename T>
	static DataPacket create(const T& value)
	{
		return create(std::make_shared<T>(value));
	}

	template<typename T>
	[[nodiscard]] std::expected<std::shared_ptr<T>, std::string> get() const noexcept
	{
		if(payloadType != typeid(T))
		{
			return std::unexpected("Error: Bad_cast");
		}

		return std::static_pointer_cast<T>(payload);
	}
};
