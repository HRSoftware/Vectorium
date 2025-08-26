#pragma once
#ifdef USE_HTTPLIB

#include "Services/REST/IPluginRESTServiceImpl.h"

#include <mutex>
#include <variant>

namespace httplib
{
	class SSLClient;
	class Client;
}

class PluginRESTService_HttpLib : public IPluginRESTServiceImpl
{
	public:
		explicit PluginRESTService_HttpLib(std::string baseUrl = "", bool bUseHttps = true);

		// Inherited via IPluginRESTService
		void set_default_headers(HeaderMap headers) override;

		void set_timeout(std::chrono::milliseconds ms) override;

		void set_bearer_token(std::string_view token) override;

		std::expected<RESTResponse, RESTError> GET(std::string_view path, const HeaderMap& headers = {}) override;

		std::expected<RESTResponse, RESTError> POST(std::string_view path,
													std::string_view body,
													const HeaderMap& headers,
													std::string_view content_type) override;
		void setBaseUrl(std::string url) override;

		void testConnection() override;


	private:
		using ClientVariant = std::variant<std::unique_ptr<httplib::Client>, 
		                                   std::unique_ptr<httplib::SSLClient>>;

		std::expected<PluginRESTService_HttpLib::ClientVariant, std::string> createRESTClient(const std::string& baseURL, bool bUseHttps = true) const;

	public:
		std::string getBaseUrl() const override;

	private:
		std::string               m_baseUrl;
		HeaderMap                 m_defaultHeaders;
		std::chrono::milliseconds m_timeout_ms{ 10000 }; //Def 10s
		std::string               m_bearer;
		bool                      m_useHttps = false;

		std::mutex m_mutex;
};

#endif
