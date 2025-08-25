#pragma once

#include "Services/UI/IUIContextProvider.h"
#include <memory>
#include <functional>

class ILogger;
struct ImGuiContext;

/// <summary>
/// Concrete implementation of IUIContextProvider
/// Manages a single ImGui context and provides safe access to it
/// Handles context switching, error recovery, and logging
/// </summary>
class UIContextProvider : public IUIContextProvider
{
public:
	explicit UIContextProvider(std::shared_ptr<ILogger> logger = nullptr);
	~UIContextProvider() override;

	// Configuration methods
	/// <summary>
	/// Sets the ImGui context that this provider will manage
	/// </summary>
	/// <param name="context">The ImGui context to store and manage</param>
	void setContext(ImGuiContext* context);

	// IUIContextProvider implementation
	ImGuiContext* getCurrentContext() override;
	bool hasValidContext() const override;
	bool executeWithContext(std::function<void()> code, 
		const std::string& contextName = "Unknown") override;
	std::string getContextInfo() const override;
	void setErrorCallback(std::function<void(const std::string&)> callback) override;

	// Additional utility methods
	/// <summary>
	/// Checks if the stored context is currently active in ImGui
	/// </summary>
	/// <returns>True if ImGui::GetCurrentContext() matches stored context</returns>
	bool isContextCurrent() const;

	/// <summary>
	/// Forces ImGui to use the stored context immediately
	/// Use with caution - prefer executeWithContext() for safe operations
	/// </summary>
	void forceSetContext();

	/// <summary>
	/// Gets the stored context without any safety checks
	/// For advanced use cases where you need direct access
	/// </summary>
	/// <returns>The stored ImGui context pointer (may be nullptr)</returns>
	ImGuiContext* getStoredContext() const { return m_context; }

private:
	std::shared_ptr<ILogger> m_logger;
	ImGuiContext* m_context;
	std::function<void(const std::string&)> m_errorCallback;

	// Logging helpers - provide consistent formatting
	void logError(const std::string& message) const;
	void logInfo(const std::string& message) const;
	void logDebug(const std::string& message) const;
	void logWarning(const std::string& message) const;

	// Internal helper for safe context operations
	bool safeSetContext(ImGuiContext* context, const std::string& operation) const;
};