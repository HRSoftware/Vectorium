#pragma once

#include <expected>
#include <memory>
#include <string>
#include <typeindex>

struct DataPacket
{
	std::shared_ptr<void> payload;
	std::type_index payloadType;

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
