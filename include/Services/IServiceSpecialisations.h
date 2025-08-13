#pragma once


#include "Services/IService.h"

// Then include all the service interfaces
#include "Services/Logging/ILogger.h"
#include "Services/Logging/SpdLogger.h"
#include "Services/REST/IRestClient.h"



template<>
struct NullObjectImpl<ILogger> : ILogger
{
	~NullObjectImpl() override = default;
	void log(LogLevel level, const std::string& message) override {}
	void enableDebugLogging() override {}
	void disableDebugLogging() override {}
};

template<>
struct NullObjectImpl<SpdLogger> : ILogger
{
	~NullObjectImpl() override = default;
	void log(LogLevel level, const std::string& message) override {}
	void enableDebugLogging() override {}
	void disableDebugLogging() override {}
};

template<>
struct NullObjectImpl<IRestClient> : IRestClient
{
	~NullObjectImpl() override = default;
	void set_default_headers(HeaderMap headers) override {}
	void set_timeout(std::chrono::milliseconds ms) override {}
	void set_bearer_token(std::string_view token) override {}
	std::expected<RESTResponse, RESTError> GET(std::string_view path, const HeaderMap& headers) override {
		return std::unexpected(RESTError{"Service not available"});
	}
	std::expected<RESTResponse, RESTError> POST(std::string_view path,
		std::string_view body,
		const HeaderMap& headers,
		std::string_view content_type) override {
		return std::unexpected(RESTError{"Service not available"});
	}
};