#include "ImGuiContextManager.h"
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GetImGuiContextFunc ImGuiContextManager::s_getContextFunc = nullptr;
SetImGuiContextFunc ImGuiContextManager::s_setContextFunc = nullptr;

struct ImGuiIO;

bool ImGuiContextManager::initialise()
{
	if (m_initialised)
	{
		std::cout << "[ImGuiContextManager] Already initialised\n";
		return true;
	}

	std::cout << "[ImGuiContextManager] Initialising ImGui context...\n";

	// Verify ImGui version
	IMGUI_CHECKVERSION();

	// Create the context
	m_context = ImGui::CreateContext();
	if (!m_context)
	{
		std::cerr << "[ImGuiContextManager] Failed to create ImGui context\n";
		return false;
	}

	// Set as current context
	ImGui::SetCurrentContext(m_context);

	// Configure ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// Add other configuration as needed

	m_initialised = true;

	std::cout << std::format("[ImGuiContextManager] ImGui context initialised successfully at address: {}\n", static_cast<void*>(m_context));

	return true;
}

void ImGuiContextManager::setContextPointer(ImGuiContext* ctx)
{
	std::cout << std::format("[ImGuiContextManager] Setting context pointer to: {}\n", static_cast<void*>(ctx));
	m_context = ctx;
	if (m_context)
	{
		m_initialised = true;
	}
}

void* ImGuiContextManager::getContextPointer() const
{
	return static_cast<void*>(m_context);
}

ImGuiContext* ImGuiContextManager::getContext() const
{
	return m_context;
}

bool ImGuiContextManager::ensureContextActive()
{
	std::cout << "[ImGuiContextManager] ensureContextActive called\n";

	if (!m_context)
	{
		std::cerr << "[ImGuiContextManager] No context available to activate\n";
		return false;
	}

	std::cout << std::format("[ImGuiContextManager] Setting current context to: {}\n", static_cast<void*>(m_context));

	ImGui::SetCurrentContext(m_context);

	ImGuiContext* current = ImGui::GetCurrentContext();
	std::cout << std::format("[ImGuiContextManager] Current context after set: {}\n", static_cast<void*>(current));

	return current == m_context;
}

bool ImGuiContextManager::isReady() const
{
	std::cout << "[ImGuiContextManager::isReady] Simple check...\n";

	// Just check if we have a context and it's current
	if (!m_initialised) 
	{
		std::cout << "[ImGuiContextManager::isReady] Not initialized\n";
		return false;
	}

	if (!m_context) {
		std::cout << "[ImGuiContextManager::isReady] No context\n";
		return false;
	}

	ImGuiContext* current = ImGui::GetCurrentContext();
	if (current != m_context) {
		std::cout << "[ImGuiContextManager::isReady] Context mismatch - Expected: "
			<< static_cast<void*>(m_context) << ", Current: " << static_cast<void*>(current) << "\n";
		return false;
	}

	std::cout << "[ImGuiContextManager::isReady] Basic checks passed\n";
	return true;
}

int ImGuiContextManager::getCurrentFrame() const
{
	if (!m_context) return -1;

	// Temporarily set context to get frame number
	ImGuiContext* prev = ImGui::GetCurrentContext();
	ImGui::SetCurrentContext(m_context);
	int frame = ImGui::GetFrameCount();
	ImGui::SetCurrentContext(prev);

	return frame;
}

void ImGuiContextManager::shutdown()
{
	if (m_context) 
	{
		std::cout << "[ImGuiContextManager] Shutting down ImGui context..." << '\n';

		// Make sure we're using our context before destroying it
		ImGui::SetCurrentContext(m_context);
		ImGui::DestroyContext(m_context);
		m_context = nullptr;
	}
	m_initialised = false;
}

void ImGuiContextManager::setContextFunctions(GetImGuiContextFunc getFunc, SetImGuiContextFunc setFunc)
{
	s_getContextFunc = getFunc;
	s_setContextFunc = setFunc;
	std::cout << "[ImGuiContextManager] Context functions set - getFunc: " 
		<< static_cast<void*>(reinterpret_cast<void*>(getFunc))
		<< ", setFunc: " << static_cast<void*>(reinterpret_cast<void*>(setFunc)) << std::endl;
}

void* ImGuiContextManager::getCurrentContextViaMainApp()
{
	std::cout << "[ImGuiContextManager] getCurrentContextViaMainApp called - s_getContextFunc: " 
		<< static_cast<void*>(reinterpret_cast<void*>(s_getContextFunc)) << std::endl;

	if (s_getContextFunc) {
		void* result = s_getContextFunc();
		std::cout << "[ImGuiContextManager] s_getContextFunc returned: " << result << std::endl;
		return result;
	}
	std::cout << "[ImGuiContextManager] s_getContextFunc is null!" << std::endl;
	return nullptr;
}

bool ImGuiContextManager::setCurrentContextViaMainApp(void* ctx)
{
	std::cout << "[ImGuiContextManager] setCurrentContextViaMainApp called with: " << ctx 
		<< ", s_setContextFunc: " << static_cast<void*>(reinterpret_cast<void*>(s_setContextFunc)) << std::endl;

	if (s_setContextFunc) {
		return s_setContextFunc(ctx);
	}
	std::cout << "[ImGuiContextManager] s_setContextFunc is null!\n";
	return false;
}