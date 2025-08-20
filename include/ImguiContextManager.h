#pragma once

struct ImGuiContext;

typedef void* (*GetImGuiContextFunc)();
typedef bool (*SetImGuiContextFunc)(void*);

class ImGuiContextManager
{
public:
	static ImGuiContextManager& getInstance()
	{
		static ImGuiContextManager instance;
		return instance;
	}

	bool          initialise();
	void setContextPointer(ImGuiContext* ctx);
	void* getContextPointer() const;

	ImGuiContext* getContext() const;
	bool          ensureContextActive();
	bool          isReady() const;
	int           getCurrentFrame() const;
	void          shutdown();

	static void setContextFunctions(GetImGuiContextFunc getFunc, SetImGuiContextFunc setFunc);
	static void* getCurrentContextViaMainApp();
	static bool setCurrentContextViaMainApp(void* ctx);

private:
	ImGuiContextManager() = default;
	~ImGuiContextManager()
	{
		shutdown();
	}

	ImGuiContextManager(const ImGuiContextManager&) = delete;
	ImGuiContextManager& operator=(const ImGuiContextManager&) = delete;


	static GetImGuiContextFunc s_getContextFunc;
	static SetImGuiContextFunc s_setContextFunc;

	ImGuiContext* m_context = nullptr;
	bool m_initialised = false;
};
