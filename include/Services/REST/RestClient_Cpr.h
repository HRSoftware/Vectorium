#pragma once

#include <mutex>

#include "IRestClient.h"

namespace cpr
{
	class Session;
}
class RESTClient_Cpr : public IRestClient
{
	public:
		RESTClient_Cpr(const std::string& baseUrl = "");
		~RESTClient_Cpr() override;
		void                                   set_default_headers(HeaderMap headers) override;
		void                                   set_timeout(std::chrono::milliseconds ms) override;
		void                                   set_bearer_token(std::string_view token) override;
		void                                   setBaseUrl(std::string url) override;
		void                                   testConnection() override;
		std::string                            getBaseUrl() const override;
		std::expected<RESTResponse, RESTError> GET(std::string_view path, const HeaderMap& headers) override;
		std::expected<RESTResponse, RESTError> POST(std::string_view path,
		std::string_view body,
		const HeaderMap& headers,
		std::string_view content_type) override;

	private:
		std::string buildFullUrl(std::string_view path) const;
		HeaderMap mergeHeaders(const HeaderMap& additional) const;
	private:
		std::string m_baseUrl;
		HeaderMap m_defaultHeaders;
		std::chrono::milliseconds m_timeout_ms{10000}; // Default 10s
		std::string m_bearer;

		std::unique_ptr<cpr::Session> m_session;
		mutable std::mutex m_mutex;
};
