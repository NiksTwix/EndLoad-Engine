#pragma once
#include <string>
#include <mutex>
#include <Systems/Render/Contexts/IRenderContext.hpp>
#include <Services\Utility\ThreadPool.hpp>


class WindowManager;
class SettingsManager;
class Render;

class Core
{
public:
	// ������ �����������
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	static Core& Get();

	bool IsRunning() { return m_isRunning;}

	// ������/��������� ������
	void Start();
	void Stop();
	void Shutdown();

private:

	ThreadPool m_ThreadPool = ThreadPool(2); //2 ������
	WindowManager* m_WinManager;
	Render* m_RenderSystem;
	std::mutex m_coreMutex;
	SettingsManager* m_SettingsManager;
	void Mainloop();

	Core();

	void Init();

	bool m_stoped = false;
	bool m_isRunning = false;

};


