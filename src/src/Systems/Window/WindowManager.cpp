#include <Systems\Window\WindowManager.hpp>
#include <Core/Core.hpp>
#include <Services/Settings/SettingsManager.hpp>
#include <Core/ServiceLocator.hpp>
#include <Systems\Window\GLFWWindow.hpp>
#include <Core/Logger.hpp>

std::shared_ptr<Window> WindowManager::CreateWindow(int width, int height, std::string name)
{
	std::shared_ptr<Window> w = nullptr;

	w = std::make_shared<GLFWWindow>(width, height, name);
	m_windows.insert({ w->GetID(), w });
	return w;
}

void WindowManager::CloseWindow(std::shared_ptr<Window> window)
{
	if (!window || !m_windows.count(window->GetID())) return;

	if (m_focusedWindow == window.get()) {
		m_focusedWindow = nullptr;
	}
	window->MakeCurrent();
	window->Close();
	m_windows.erase(window->GetID());
	ServiceLocator::Get<Logger>()->Log("(WindowManager): window with id " + std::to_string(window->GetID()) + " has been deleted.", Logger::Level::Warning);
}
 
void WindowManager::UpdateAllWindows()
{
	for (auto p : m_windows) 
	{
		p.second->UpdateState();
	}
}

void WindowManager::Update()
{
	UpdateAllWindows();
}

void WindowManager::Init()
{
	if (m_isValid) return;
	m_isValid = true;
}

void WindowManager::Shutdown()
{
	if (!m_isValid) {
		// Логируем предупреждение, если Shutdown() вызван повторно
		ServiceLocator::Get<Logger>()->Log(
			"(WindowManager): double shutdown attempt!",
			Logger::Level::Warning
		);
		return;
	}

	// Логируем количество окон, которые будут закрыты
	size_t windowCount = m_windows.size();
	auto logger = ServiceLocator::Get<Logger>();
	logger->Log(
		"(WindowManager): shutting down " + std::to_string(windowCount) + " windows...",
		Logger::Level::Info
	);

	// Уничтожаем все окна
	m_windows.clear();  // shared_ptr автоматически вызовут деструкторы

	//TODO УБРАТЬ
	// Завершаем GLFW (если он был инициализирован)
	if (glfwInit()) {  // Проверяем, был ли GLFW активен
		glfwTerminate();
		logger->Log("(WindowManager): GLFW terminated.", Logger::Level::Info);
	}

	m_isValid = false;
	logger->Log("(WindowManager): shutdown complete.", Logger::Level::Warning);
}

WindowManager::~WindowManager()
{
}

std::shared_ptr<Window> WindowManager::GetWindow(WindowID id) const
{
	if (auto t = m_windows.find(id); t == m_windows.end()) return nullptr;
	return m_windows.at(id);
}

bool WindowManager::IsWindowValid(WindowID id) const
{
	if (auto t = m_windows.find(id); t == m_windows.end()) return false;
	return true;
}
