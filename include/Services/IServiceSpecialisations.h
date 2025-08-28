#pragma once
#include "Services/IService.h"

// Logging
#include "Services/Logging/ILogger.h"
#include "Logging/SpdLogger.h"

// REST
#include "Services/REST/IPluginRESTService.h"
#ifdef USE_CPR
#include "Services/REST/PluginRESTService_Cpr.h"
#endif
#ifdef USE_HTTPLIB
#include "Services/REST/PluginRESTService_HttpLib.h"
#endif

// UI
#include "../../UI/include/Services/UI/IPluginUIService.h"
#include "../../UI/include/Services/UI/PluginUIService_ImGui.h"

class PluginUIService_ImGui;

//	Logging
template<>
struct NullObjectImpl<ILogger> : ILogger
{
	~NullObjectImpl() override = default;
	void log([[maybe_unused]] LogLevel level, [[maybe_unused]] const std::string& message) override {}
	void enableDebugLogging() override {}
	void disableDebugLogging() override {}
	bool isDebugLoggingEnabled() const override { return false; }
	void setPluginName([[maybe_unused]] const std::string& name) override {}
};

template<>
struct NullObjectImpl<SpdLogger> : ILogger
{
	~NullObjectImpl() override = default;
	void log([[maybe_unused]] LogLevel level, [[maybe_unused]] const std::string& message) override {}
	void enableDebugLogging() override {}
	void disableDebugLogging() override {}
	bool isDebugLoggingEnabled() const override { return false; }
	void setPluginName([[maybe_unused]] const std::string& name) override {}
};

//	REST
template<>
struct NullObjectImpl<IPluginRESTService> : IPluginRESTService
{
	~NullObjectImpl() override = default;
	void set_default_headers([[maybe_unused]] HeaderMap headers) override {}
	void set_timeout([[maybe_unused]] std::chrono::milliseconds ms) override {}
	void set_bearer_token([[maybe_unused]] std::string_view token) override {}

	std::expected<RESTResponse, RESTError> GET([[maybe_unused]] std::string_view path, [[maybe_unused]] const HeaderMap& headers) override
	{
		return std::unexpected(RESTError{"Service not available"});
	}

	std::expected<RESTResponse, RESTError> POST([[maybe_unused]] std::string_view path,
		[[maybe_unused]] std::string_view body,
		[[maybe_unused]] const HeaderMap& headers,
		[[maybe_unused]] std::string_view content_type) override {
		return std::unexpected(RESTError{"Service not available"});
	}

	void        setBaseUrl([[maybe_unused]] std::string url) override {}
	std::string getBaseUrl() const override { return ""; }
	void        testConnection() override {}
};

#ifdef USE_HTTPLIB
template<>
struct NullObjectImpl<PluginRESTService_HttpLib> : IPluginRESTService
{
	~NullObjectImpl() override = default;
	void set_default_headers([[maybe_unused]] HeaderMap headers) override {}
	void set_timeout([[maybe_unused]] std::chrono::milliseconds ms) override {}
	void set_bearer_token([[maybe_unused]] std::string_view token) override {}

	std::expected<RESTResponse, RESTError> GET([[maybe_unused]] std::string_view path, [[maybe_unused]] const HeaderMap& headers) override
	{
		return std::unexpected(RESTError{"Service not available"});
	}

	std::expected<RESTResponse, RESTError> POST([[maybe_unused]] std::string_view path,
		[[maybe_unused]] std::string_view body,
		[[maybe_unused]] const HeaderMap& headers,
		[[maybe_unused]] std::string_view content_type) override {
		return std::unexpected(RESTError{"Service not available"});
	}

	void setBaseUrl([[maybe_unused]] std::string url) override {}

	// ADD THIS: Explicit template instantiation
	template class NullService<PluginRESTService_HttpLib>;

};
#endif


#ifdef USE_CPR
template<>
struct NullObjectImpl<PluginRESTService_Cpr> : IPluginRESTService
{
	~NullObjectImpl() override = default;
	void set_default_headers([[maybe_unused]] HeaderMap headers) override {}
	void set_timeout([[maybe_unused]] std::chrono::milliseconds ms) override {}
	void set_bearer_token([[maybe_unused]] std::string_view token) override {}

	std::expected<RESTResponse, RESTError> GET([[maybe_unused]] std::string_view path, [[maybe_unused]] const HeaderMap& headers) override
	{
		return std::unexpected(RESTError{ "Service not available" });
	}

	std::expected<RESTResponse, RESTError> POST([[maybe_unused]] std::string_view path,
		[[maybe_unused]] std::string_view body,
		[[maybe_unused]] const HeaderMap& headers,
		[[maybe_unused]] std::string_view content_type) override {
		return std::unexpected(RESTError{ "Service not available" });
	}

	void setBaseUrl([[maybe_unused]] std::string url) override {}


	template class NullService<PluginRESTService_Cpr>;
};
#endif


// UI
template<>
struct NullObjectImpl<IPluginUIService> : IPluginUIService
{
	ImGuiContext* getImGuiContext() override { return nullptr; }
	void          setImGuiContext([[maybe_unused]] ImGuiContext* context) override {}
	void          registerPluginUIRenderer([[maybe_unused]] const std::string& pluginName, [[maybe_unused]] std::function<void()> renderCallback) override {}
	void          unregisterPluginUIRenderer([[maybe_unused]] const std::string& pluginName) override {}
	void          renderPluginUIs() override {}
	bool          isContextValid() const override { return false; }
	std::string   getDiagnosticInfo() const override { return {}; }
	size_t        getRegisteredPluginCount() const override { return 0; }
	void          setErrorCallback([[maybe_unused]] std::function<void(const std::string&)> callback) override {}
	bool isUIAvailable() const override { return false; }
};
