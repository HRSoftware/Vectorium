#pragma once

#ifdef _WIN32
#define EXPORT extern "C" __declspec(dllexport)
#else
#define EXPORT extern "C"
#endif

struct GPSData
{
	float lat;
	float lng;
	float alt;
};
