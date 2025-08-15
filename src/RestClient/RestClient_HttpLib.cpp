#include "Services/REST/RestClient_HttpLib.h"

#include <string>
#include <utility>
#include <httplib.h>


RESTClient_HttpLib::RESTClient_HttpLib(std::string baseUrl, bool bUseHttps) : m_baseUrl(std::move(baseUrl)), m_useHttps(bUseHttps)
{}

static void apply_headers(httplib::Headers& dst, const HeaderMap& src)
{
	//why do this?
	for (auto& [k,v] : src)
	{
		dst.emplace(k, v);
	};
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

	//std::unique_ptr<httplib::Client> cli = m_useHttps ? std::make_unique<httplib::SSLClient>(m_baseUrl.c_str()) : std::make_unique<httplib::Client>(m_baseUrl.c_str());
	std::unique_ptr<httplib::Client> cli = std::make_unique<httplib::Client>(m_baseUrl.c_str());
	cli->set_connection_timeout(m_timeout_ms.count() / 1000, m_timeout_ms.count() % 1000 * 1000);

	auto res = cli->Get(std::string(path), tmpHeaders);

	if(!res)
	{
		return std::unexpected(RESTError{res.error() == httplib::Error::Success ? "Unknown error" : httplib::to_string(res.error())});
	}

	RESTResponse out;
	out.status = res->status;
	for(auto& [k, v] : res->headers)
	{
		out.headers.emplace(k, v);
	}

	out.body.assign(res->body.begin(), res->body.end());
	return out;
}

std::expected<RESTResponse, RESTError> RESTClient_HttpLib::POST(std::string_view path,
	std::string_view body,
	const HeaderMap& headers,
	std::string_view content_type)
{
	std::scoped_lock lk(m_mutex);
	httplib::Headers tmpHeaders;
	apply_headers(tmpHeaders, m_defaultHeaders);
	apply_headers(tmpHeaders, headers);

	/*std::unique_ptr<httplib::Client> cli = m_useHttps;
		? std::make_unique<httplib::SSLClient>(m_baseUrl.c_str())
		: std::make_unique<httplib::Client>(m_baseUrl.c_str());*/
	std::unique_ptr<httplib::Client> cli = std::make_unique<httplib::Client>(m_baseUrl.c_str());
	cli->set_connection_timeout(m_timeout_ms.count()/1000, m_timeout_ms.count()%1000*1000);

	auto res = cli->Post(std::string(path), tmpHeaders, std::string(body), std::string(content_type));
	if (!res) return std::unexpected(RESTError{httplib::to_string(res.error())});

	RESTResponse out;
	out.status = res->status;
	for (auto& hkv : res->headers) out.headers.emplace(hkv.first, hkv.second);
	out.body.assign(res->body.begin(), res->body.end());
	return out;
}

void RESTClient_HttpLib::setBaseUrl(const std::string url)
{
	m_baseUrl = url;
}
