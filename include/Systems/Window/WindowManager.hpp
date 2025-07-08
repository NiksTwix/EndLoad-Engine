#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <Core/ISystem.hpp>
#include <Systems/Window/Window.hpp>



class WindowManager : public ISystem
{
public:
	WindowManager() { Init(); };

	std::shared_ptr<Window> CreateWindow(int width, int height, std::string name);

	void CloseWindow(std::shared_ptr<Window> window);

	void UpdateAllWindows();
	void Update() override;
	void Init();

	void Shutdown() override; //Удаляет все окна

	virtual SystemPriority GetPriority() override { return SystemPriority::WINDOW_MANAGER; } //Наивысший приоритет

	int GetWindowsCount() { return m_windows.size(); };

	~WindowManager();
	
	void SetFocusedWindow(Window* window) { m_focusedWindow = window; }
	void SetRenderWindow(Window* window) {
		window->MakeCurrent();
		m_renderWindow = window; }

	Window* GetFocusedWindow() const { return m_focusedWindow; }
	Window* GetRenderWindow() const { return m_renderWindow; }

	std::shared_ptr<Window> GetWindow(WindowID id) const;
	bool IsWindowValid(WindowID id) const;

private:
	std::unordered_map<WindowID,std::shared_ptr<Window>> m_windows;
	Window* m_focusedWindow = nullptr;  // Окно с фокусом ввода
	Window* m_renderWindow = nullptr;   // Окно, в котором идёт рендер
};