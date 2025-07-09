#include "Mocking/MockLogger.h"

#include <iostream>

#include "Logger/LogLevel.h"

inline void MockLogger::log(LogLevel level, const std::string& message)
{
	std::cout << "[MockLogger][" << static_cast<int>(level) << "] " << message << "\n";
}
