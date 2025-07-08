#include <Core/Core.hpp>
#include <Core/ServiceLocator.hpp>
#include <Core/Logger.hpp>
#include <Systems/Window/WindowManager.hpp>

#include <Systems/Render/Render.hpp>
#include <Systems/Render/Contexts/OpenGLContext.hpp>

#include <Systems/Physics/Physics.hpp>
#include <Systems/Input/Input.hpp>
#include <Services/Settings/SettingsManager.hpp>
#include <Services/Resources/ResourceManager.hpp>
#include <Services\Scene\SceneManager.hpp>
#include <Services/Events/EventManager.hpp>
#include <Services\Scene\SceneLoader.hpp>
#include <Services\Serialization\SerializationService.hpp>


Core& Core::Get()
{
	static Core instance; // Ленивая инициализация. Объект создаётся только в первый раз
	return instance;
}

void Core::Start() //Запускает/перезапускает цикл
{
	m_stoped = false;
	Mainloop();
}

void Core::Stop()	//Полностью останавливает цикл
{
	m_stoped = true;
}

void Core::Shutdown()
{
	ServiceLocator::Shutdown();
}

void Core::Mainloop()
{
	if (IsRunning()) return;
	try 
	{	//TODO Привязать сцены к окнам
		m_isRunning = true;
		while (m_WinManager->GetWindowsCount() != 0 && !m_stoped)
		{
			for (auto system : ServiceLocator::GetSystemList())
			{
				if (system->IsNeedAsync()) m_ThreadPool.Enqueue([system]() {system->Update(); }); //Асинхронное выполнение
				else 
				{
					system->Update(); //Синхронное выполнение
				}
			}
		}
		//Запуск физики в отдельном потоке Создать задачу PhysicsSystem -> Update
		//Запуск графики в отдельном потоке
	}
	catch (std::exception& e) 
	{
		ServiceLocator::Get<Logger>()->Log(e.what(), Logger::Level::Error);
		m_isRunning = false;
	}
	m_isRunning = false;
}

Core::Core()
{
	Init();
}

void Core::Init()
{
	ServiceLocator::Set<Logger, Logger>();
	auto logger = ServiceLocator::Get<Logger>();
	logger->Log("Core initialization has started.");


	ServiceLocator::Set<EventManager, EventManager>();
	ServiceLocator::Set<SettingsManager, SettingsManager>();
	ServiceLocator::Set<ResourceManager, ResourceManager>();
	ServiceLocator::Set<WindowManager, WindowManager>();
	ServiceLocator::Set<Physics, Physics>();
	ServiceLocator::Set<Render, Render>();
	ServiceLocator::Set<Input, Input>();
	ServiceLocator::Set<SerializationService, SerializationService>();
	ServiceLocator::Set<SceneManager, SceneManager>();
	ServiceLocator::Set<SceneLoader, SceneLoader>();
	


	m_WinManager = ServiceLocator::Get<WindowManager>();
	m_RenderSystem = ServiceLocator::Get<Render>();
	m_SettingsManager = ServiceLocator::Get<SettingsManager>();
	
	if (m_SettingsManager->GetRenderType() == RenderTypes::OpenGL) 
	{
		m_RenderSystem->SetContext(std::unique_ptr<IRenderContext>(new OpenGLContext()));
	}
	
	logger->Log("Core initialization has ended.");
}

