#include "Services/REST/PluginRESTService_Cpr.h"

#include <cpr/cpr.h>
#include <cpr/session.h>

namespace
{
	RESTResponse convertResponse(const void* cpr_response_ptr)
	{
		const auto& cpr_response = *static_cast<const cpr::Response*>(cpr_response_ptr);

		RESTResponse response;
		response.status = static_cast<int>(cpr_response.status_code);

		// Convert headers
		for (const auto& [key, value] : cpr_response.header)
		{
			response.headers[key] = value;
		}

		// Convert body
		const std::string& body_str = cpr_response.text;
		response.body.assign(body_str.begin(), body_str.end());

		return response;
	}
}

PluginRESTService_Cpr::PluginRESTService_Cpr(const std::string& baseUrl)
: m_baseUrl(baseUrl)
, m_session(std::make_unique<cpr::Session>())
{
}

PluginRESTService_Cpr::~PluginRESTService_Cpr() = default;

void PluginRESTService_Cpr::set_default_headers(HeaderMap headers)
{
	std::scoped_lock lock(m_mutex);
	m_defaultHeaders = headers;
}

void PluginRESTService_Cpr::set_timeout(std::chrono::milliseconds ms)
{
	std::scoped_lock lock(m_mutex);
	m_timeout_ms = ms;
}

void PluginRESTService_Cpr::set_bearer_token(std::string_view token)
{
	std::scoped_lock lock(m_mutex);
	m_bearer = token;
}

void PluginRESTService_Cpr::setBaseUrl(std::string url)
{
	std::scoped_lock lock(m_mutex);
	m_baseUrl = url;
}

void PluginRESTService_Cpr::testConnection()
{
}

std::string PluginRESTService_Cpr::getBaseUrl() const
{
	return m_baseUrl;
}

std::expected<RESTResponse, RESTError> PluginRESTService_Cpr::GET(std::string_view path, const HeaderMap& headers)
{
	std::scoped_lock lock(m_mutex);

	try
	{
		const std::string url = buildFullUrl(path);
		const HeaderMap mergedHeaders = mergeHeaders(headers);

		cpr::Header cprHeaders;
		for(const auto&[key, value] : mergedHeaders)
		{
			cprHeaders[key] = value;
		}

		auto response = cpr::Get(cpr::Url{ url },
			cprHeaders,
			cpr::Timeout{ m_timeout_ms });

		if(response.error.code != cpr::ErrorCode::OK)
		{
			return std::unexpected(RESTError{ response.error.message });
		}

		return convertResponse(&response);
	}
	catch(const std::exception& e)
	{
		return std::unexpected(RESTError{std::string("Exception: ") + e.what()});
	}
}

std::expected<RESTResponse, RESTError> PluginRESTService_Cpr::POST(std::string_view path,
	std::string_view body,
	const HeaderMap& headers,
	std::string_view content_type)
{
	return {};
}

std::string PluginRESTService_Cpr::buildFullUrl(std::string_view path) const
{
	if (path.starts_with("http"))
	{
		return std::string(path); // it's already a url
	}

	std::string result = m_baseUrl;
	if (!result.empty() && !result.ends_with('/') && !path.starts_with('/'))
	{
		result += '/';
	}
	result += path;
	return result;
}

HeaderMap PluginRESTService_Cpr::mergeHeaders(const HeaderMap& additional) const
{
	HeaderMap merged = m_defaultHeaders;
	for (const auto& [key, value] : additional)
	{
		merged[key] = value;
	}
	return merged;
}


