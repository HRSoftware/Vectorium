//#include "Services/UI/UIContextProvider.h"
//#include "Services/Logging/ILogger.h"
//#include "Services/Logging/LogLevel.h"
//#include <imgui.h>
//#include <format>
//
//#include "Services/UI/PluginUIService_ImGui.h"
//
//UIContextProvider::UIContextProvider(std::shared_ptr<ILogger> logger)
//	: m_logger(std::move(logger))
//	, m_context(nullptr)
//{
//	logDebug("UIContextProvider created");
//}
//
//UIContextProvider::~UIContextProvider()
//{
//	logDebug("UIContextProvider destroyed");
//}
//
//void UIContextProvider::setContext(ImGuiContext* context)
//{
//	m_context = context;
//
//	if (context) {
//		logInfo(std::format("Context set to: {}", static_cast<void*>(context)));
//	} else {
//		logInfo("Context set to nullptr");
//	}
//}
//
//ImGuiContext* UIContextProvider::getCurrentContext()
//{
//	return m_context;
//}
//
//bool UIContextProvider::hasValidContext() const
//{
//	return m_context != nullptr;
//}
//
//bool UIContextProvider::executeWithContext(std::function<void()> code, 
//	const std::string& contextName)
//{
//	if (!hasValidContext()) {
//		logError(std::format("Cannot execute '{}': no valid context available", contextName));
//		return false;
//	}
//
//	if (!code) {
//		logError(std::format("Cannot execute '{}': no code provided", contextName));
//		return false;
//	}
//
//	// Store current context to restore later
//	ImGuiContext* previousContext = ImGui::GetCurrentContext();
//
//	try {
//		// Set our context if it's different
//		if (previousContext != m_context) {
//			logDebug(std::format("Switching context for '{}': {} -> {}", 
//				contextName, 
//				static_cast<void*>(previousContext), 
//				static_cast<void*>(m_context)));
//
//			ImGui::SetCurrentContext(m_context);
//		}
//
//		// Execute the code
//		code();
//
//		// Restore previous context if needed
//		if (previousContext != m_context) {
//			logDebug(std::format("Restoring context after '{}': {} -> {}", 
//				contextName,
//				static_cast<void*>(m_context),
//				static_cast<void*>(previousContext)));
//
//			ImGui::SetCurrentContext(previousContext);
//		}
//
//		return true;
//
//	} catch (const std::exception& e) {
//		logError(std::format("Exception during execution of '{}': {}", contextName, e.what()));
//
//		// Try to restore context on error
//		if (previousContext != m_context) {
//			try {
//				ImGui::SetCurrentContext(previousContext);
//				logDebug(std::format("Context restored after exception in '{}'", contextName));
//			} catch (const std::exception& restoreEx) {
//				logError(std::format("Failed to restore context after exception in '{}': {}", 
//					contextName, restoreEx.what()));
//
//				if (m_errorCallback) {
//					m_errorCallback(std::format("Critical: Context restoration failed in {}", contextName));
//				}
//			}
//		}
//
//		return false;
//	} catch (...) {
//		logError(std::format("Unknown exception during execution of '{}'", contextName));
//
//		// Try to restore context on error
//		if (previousContext != m_context) {
//			try {
//				ImGui::SetCurrentContext(previousContext);
//				logDebug(std::format("Context restored after unknown exception in '{}'", contextName));
//			} catch (...) {
//				logError(std::format("Failed to restore context after unknown exception in '{}'", contextName));
//
//				if (m_errorCallback) {
//					m_errorCallback(std::format("Critical: Context restoration failed in {}", contextName));
//				}
//			}
//		}
//
//		return false;
//	}
//}
//
//std::string UIContextProvider::getContextInfo() const
//{
//	ImGuiContext* currentContext = ImGui::GetCurrentContext();
//
//	return std::format("UIContextProvider - Stored: {}, Current: {}, Valid: {}", 
//		static_cast<void*>(m_context),
//		static_cast<void*>(currentContext),
//		hasValidContext());
//}
//
//void UIContextProvider::setErrorCallback(std::function<void(const std::string&)> callback)
//{
//	m_errorCallback = std::move(callback);
//	logDebug("Error callback set");
//}
//
//bool UIContextProvider::isContextCurrent() const
//{
//	if (!hasValidContext()) {
//		return false;
//	}
//
//	return ImGui::GetCurrentContext() == m_context;
//}
//
//void UIContextProvider::forceSetContext()
//{
//	if (hasValidContext()) {
//		ImGui::SetCurrentContext(m_context);
//		logDebug("Context forcibly set to stored context");
//	} else {
//		logWarning("Cannot force set context - no valid stored context");
//	}
//}
//
//// Private logging helpers
//void UIContextProvider::logError(const std::string& message) const
//{
//	if (m_logger) {
//		m_logger->log(LogLevel::Error, std::format("[UIContextProvider] {}", message));
//	}
//}
//
//void UIContextProvider::logInfo(const std::string& message) const
//{
//	if (m_logger) {
//		m_logger->log(LogLevel::Info, std::format("[UIContextProvider] {}", message));
//	}
//}
//
//void UIContextProvider::logDebug(const std::string& message) const
//{
//	if (m_logger) {
//		m_logger->log(LogLevel::Debug, std::format("[UIContextProvider] {}", message));
//	}
//}
//
//void UIContextProvider::logWarning(const std::string& message) const
//{
//	if (m_logger) {
//		m_logger->log(LogLevel::Warning, std::format("[UIContextProvider] {}", message));
//	}
//}