#pragma once

#include <functional>
#include <string>

struct ImGuiContext;

/// <summary>
/// Interface for providing ImGui context to different consumers
/// Separates context management from service provision
/// </summary>
class IUIContextProvider
{
public:
	virtual ~IUIContextProvider() = default;

	/// <summary>
	/// Gets the current ImGui context for use
	/// </summary>
	/// <returns>Pointer to the ImGui context, or nullptr if not available</returns>
	virtual ImGuiContext* getCurrentContext() = 0;

	/// <summary>
	/// Checks if a valid context is available
	/// </summary>
	/// <returns>True if context is available and valid</returns>
	virtual bool hasValidContext() const = 0;

	/// <summary>
	/// Executes code with the provided context safely set
	/// Handles context switching and restoration automatically
	/// </summary>
	/// <param name="code">Function to execute with context</param>
	/// <param name="contextName">Name for debugging/logging</param>
	/// <returns>True if execution was successful</returns>
	virtual bool executeWithContext(std::function<void()> code, 
		const std::string& contextName = "Unknown") = 0;

	/// <summary>
	/// Gets context information for debugging
	/// </summary>
	/// <returns>String describing context state</returns>
	virtual std::string getContextInfo() const = 0;

	/// <summary>
	/// Sets callback for context-related errors
	/// </summary>
	/// <param name="callback">Function to call on context errors</param>
	virtual void setErrorCallback(std::function<void(const std::string&)> callback) = 0;
};