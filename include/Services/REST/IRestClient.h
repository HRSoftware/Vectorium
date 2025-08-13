#pragma once
#include <chrono>
#include <expected>
#include <map>
#include <string>
#include <vector>

struct RESTResponse
{
	int status = -1;
	std::map<std::string, std::string> headers;
	std::vector<unsigned char> body; //binary safe
};

struct RESTError
{
	std::string error;
};

namespace
{
	using HeaderMap = std::map<std::string, std::string>;
}


class IRestClient
{
public:
	virtual ~IRestClient() = default;

	virtual void set_default_headers(HeaderMap headers) = 0;
	virtual void set_timeout(std::chrono::milliseconds ms) = 0;
	virtual void set_bearer_token(std::string_view token) = 0;  // convenience

	virtual std::expected<RESTResponse, RESTError>
		GET(std::string_view path, const HeaderMap& headers = {}) = 0;

	virtual std::expected<RESTResponse, RESTError>
		POST(std::string_view path, std::string_view body,
			const HeaderMap& headers = {}, std::string_view content_type = "application/json") = 0;

	// Add put/patch/delete as needed
};