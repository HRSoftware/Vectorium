
#pragma once

#include <ranges>

template<typename Range>
std::string join_range(const Range& range, const std::string& delimiter = ", ")
{
	std::ostringstream oss;
	auto it = std::ranges::begin(range), end = std::ranges::end(range);

	if (it != end) oss << *it++;
	while (it != end) oss << delimiter << *it++;
	return oss.str();
}