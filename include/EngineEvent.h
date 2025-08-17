#pragma once
#include <chrono>
#include <unordered_map>

enum class EngineEventType
{

};

class EngineEvent
{
	public:
		EngineEventType type;
		std::chrono::system_clock::time_point timestamp;
		std::unordered_map<std::string, std::any> data;

		template<typename T>
		T getData(const std::string& key) const
		{
			auto it = data.find(key);
			if (it != data.end())
			{
				return std::any_cast<T>(it->second);
			}
			throw std::runtime_error("Key not found: " + key);
		}

		template<typename T>
		std::optional<T> tryGetData(const std::string& key) const
		{
			auto it = data.find(key);
			if (it != data.end())
			{
				try {
					return std::any_cast<T>(it->second);
				} catch (const std::bad_any_cast&)
				{
					return std::nullopt;
				}
			}

			return std::nullopt;
		}
};