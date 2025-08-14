#pragma once

struct ServiceId
{
	std::type_index type;
	std::string     name;      // stable string id
	std::string     minVersion; // semver-ish, e.g., ">=1.2.0"
	bool            required{true};
};