#include "Services/REST/RestClient_HttpLib.h"

#include <string>
#include <utility>
#include <httplib.h>


RESTClient_HttpLib::RESTClient_HttpLib(std::string baseUrl, bool bUseHttps) : m_baseUrl(std::move(baseUrl)), m_useHttps(bUseHttps)
{
	//Auto-detect if not explicitly set
	m_useHttps = m_baseUrl.starts_with("https://");
}

static void apply_headers(httplib::Headers& dst, const HeaderMap& src)
{
	//why do this?
	for (auto& [k,v] : src)
	{
		dst.emplace(k, v);
	}
}



void RESTClient_HttpLib::set_default_headers(HeaderMap headers)
{
	std::scoped_lock lk(m_mutex);
	m_defaultHeaders = std::move(headers);
}

void RESTClient_HttpLib::set_timeout(std::chrono::milliseconds ms)
{
	std::scoped_lock lk(m_mutex);
	m_timeout_ms = ms;
}

void RESTClient_HttpLib::set_bearer_token(std::string_view token)
{
	std::scoped_lock lk(m_mutex);
	m_bearer.assign(token);
	if(!m_bearer.empty())
	{
		m_defaultHeaders["Authorization"] = "Bearer " + m_bearer;
	}
}

std::expected<RESTResponse, RESTError> RESTClient_HttpLib::GET(std::string_view path, const HeaderMap& headers)
{
	std::scoped_lock lk(m_mutex);
	httplib::Headers tmpHeaders;
	apply_headers(tmpHeaders, m_defaultHeaders);
	apply_headers(tmpHeaders, headers);

	auto createREST_result = createRESTClient(m_baseUrl);

	if(!createREST_result.has_value())
	{
		return std::unexpected(RESTError{ createREST_result.error() });
	}

	auto& clientVariant = createREST_result.value();
	auto GetResponse = std::visit([&](auto& cli) -> std::expected<RESTResponse, RESTError>
	{
		cli->set_connection_timeout(m_timeout_ms.count()/1000, m_timeout_ms.count()%1000*1000);

		auto response = cli->Get(std::string(path), tmpHeaders);
		if (!response)
		{
			return std::unexpected(RESTError{httplib::to_string(response.error())});
		}

		RESTResponse out;
		out.status = response->status;
		for (auto& hkv : response->headers) out.headers.emplace(hkv.first, hkv.second);
		out.body.assign(response->body.begin(), response->body.end());
		return out;
	}, clientVariant);

	return GetResponse;
}

std::expected<RESTResponse, RESTError> RESTClient_HttpLib::POST(const std::string_view path,
	std::string_view body,
	const HeaderMap& headers,
	std::string_view content_type)
{
	std::scoped_lock lk(m_mutex);
	httplib::Headers tmpHeaders;
	apply_headers(tmpHeaders, m_defaultHeaders);
	apply_headers(tmpHeaders, headers);

	auto createRESTClient_result = createRESTClient(m_baseUrl);

	if(!createRESTClient_result.has_value())
	{
		return std::unexpected(RESTError{createRESTClient_result.error()});
	}

	auto& clientVariant = createRESTClient_result.value();
	auto PostResponse = std::visit([&](auto& cli) -> std::expected<RESTResponse, RESTError>
	{
		cli->set_connection_timeout(m_timeout_ms.count()/1000, m_timeout_ms.count()%1000*1000);

		auto response = cli->Post(std::string(path), tmpHeaders, std::string(body), std::string(content_type));
		if (!response)
		{
			return std::unexpected(RESTError{httplib::to_string(response.error())});
		}

		RESTResponse out;
		out.status = response->status;
		for (auto& hkv : response->headers) out.headers.emplace(hkv.first, hkv.second);
		out.body.assign(response->body.begin(), response->body.end());
		return out;
	}, clientVariant);

	return PostResponse;
}

void RESTClient_HttpLib::setBaseUrl(const std::string url)
{
	m_useHttps = url.starts_with("https://");
	m_baseUrl = url;
}

std::expected<RESTClient_HttpLib::ClientVariant, std::string> RESTClient_HttpLib::createRESTClient(
	const std::string& baseURL) const
{
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
	if (m_useHttps)
	{
		return std::make_unique<httplib::SSLClient>(baseURL.c_str());
	} else
	{
		return  std::make_unique<httplib::Client>(baseURL.c_str());
	}
#else
	if (m_useHttps)
	{
		 return std::unexpected( "HTTPS not supported - OpenSSL support not compiled in");
	}

	return std::make_unique<httplib::Client>(baseURL.c_str());
#endif
}
