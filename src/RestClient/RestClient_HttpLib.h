#pragma once

#include "Services/REST/IRestClient.h"

#include <mutex>

class RESTClient_HttpLib : public IRestClient
{
	public:
		explicit RESTClient_HttpLib(std::string baseUrl, bool bUseHttps = true);

		// Inherited via IRestClient
		void set_default_headers(HeaderMap headers) override;

		void set_timeout(std::chrono::milliseconds ms) override;

		void set_bearer_token(std::string_view token) override;

		std::expected<RESTResponse, RESTError> GET(std::string_view path, const HeaderMap& headers = {}) override;

		std::expected<RESTResponse, RESTError> POST(std::string_view path,
													std::string_view body,
													const HeaderMap& headers,
													std::string_view content_type) override;

	private:
		std::string m_baseUrl;
		HeaderMap m_defaultHeaders;
		std::chrono::milliseconds m_timeout_ms{ 10000 }; //Def 10s
		std::string m_bearer;
		bool m_useHttps = false;

		std::mutex m_mutex;
};