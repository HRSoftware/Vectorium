#pragma once
#include <chrono>
#include <expected>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

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

namespace RESTUtils
{
	inline std::string bodyToString(const std::vector<unsigned char>& body)
	{
		if (body.empty())
		{
			return "";
		}
		return {body.begin(), body.end()};
	}

	inline std::string bodyToStringSafe(const std::vector<unsigned char>& body)
	{
		if (body.empty()) {
			return "<empty>";
		}

		// Check if it's likely text
		const bool isPrintable = std::ranges::all_of(body,
		                                       [](const unsigned char c)
		                                       {
			                                       return std::isprint(c) || std::isspace(c);
		                                       });

		if (isPrintable)
		{
			return std::string(body.begin(), body.end());
		}

		return std::format("<binary data, {} bytes>", body.size());
	}

	inline std::string bodyToStringTruncated(const std::vector<unsigned char>& body, size_t maxLength = 500)
	{
		if (body.empty())
		{
			return "<empty>";
		}

		std::string bodyStr(body.begin(), body.end());

		if (bodyStr.length() <= maxLength) {
			return bodyStr;
		}

		return bodyStr.substr(0, maxLength) + std::format("... (truncated, total {} chars)", bodyStr.length());
	}

	inline bool isTextResponse(const RESTResponse& response)
	{
		// Check Content-Type header
		const auto it = response.headers.find("content-type");
		if (it != response.headers.end())
		{
			const std::string& contentType = it->second;
			return contentType.starts_with("text/") ||
				contentType.find("application/json") != std::string::npos ||
				contentType.find("application/xml") != std::string::npos;
		}

		// Fallback to content inspection
		return std::ranges::all_of(response.body,
		                           [](unsigned char c) { return std::isprint(c) || std::isspace(c); });
	}

	inline std::string formatForLogging(const RESTResponse& response, size_t maxLength = 500)
	{
		if (response.body.empty())
		{
			return std::format("Status: {}, <empty body>", response.status);
		}

		if (isTextResponse(response))
		{
			std::string body = bodyToStringTruncated(response.body, maxLength);
			return std::format("Status: {}, Body: {}", response.status, body);
		}

		return std::format("Status: {}, <binary data, {} bytes>", response.status, response.body.size());
	}
}

/// <summary>
/// Defines an interface for a REST client, providing methods to configure the client and perform HTTP requests.
/// </summary>
class IRestClient
{
public:
	virtual ~IRestClient() = default;

	virtual void set_default_headers(HeaderMap headers) = 0;
	virtual void set_timeout(std::chrono::milliseconds ms) = 0;
	virtual void set_bearer_token(std::string_view token) = 0;  // convenience
	virtual void setBaseUrl(std::string url) = 0;

	virtual std::expected<RESTResponse, RESTError>
		GET(std::string_view path, const HeaderMap& headers = {}) = 0;

	virtual std::expected<RESTResponse, RESTError>
		POST(std::string_view path, std::string_view body,
			const HeaderMap& headers = {}, std::string_view content_type = "application/json") = 0;

	// Add put/patch/delete as needed
};