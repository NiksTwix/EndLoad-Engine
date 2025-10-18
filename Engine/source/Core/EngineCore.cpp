#include <Core/ServiceLocator.hpp>
#include <Systems/Input/InputSystem.hpp>
#include <Core/EngineCore.hpp>

using namespace Core;

EngineCore& EngineCore::Get()
{
	static EngineCore instance; // Ленивая инициализация. Объект создаётся только в первый раз
	return instance;
}

void EngineCore::Start() //Запускает/перезапускает цикл
{
	m_stoped = false;
	Mainloop();
}

void EngineCore::Stop()	//Полностью останавливает цикл
{
	m_stoped = true;
}

void EngineCore::Shutdown()
{
	ServiceLocator::Shutdown();
}

void EngineCore::Mainloop()
{
	if (IsRunning()) return;
	try
	{
		m_isRunning = true;
		while (!m_stoped)
		{
			for (auto system : ServiceLocator::GetSystemList())
			{
				if (system->IsNeedAsync()) m_ThreadPool.Enqueue([system]() {system->Update(); }); //Async 
				else
				{
					system->Update(); //Sync
				}
			}

			m_ThreadPool.WaitForCompletion();

			Diagnostics::ProcessObserver::Get().CalculateDelta();

			
		}
		//Запуск физики в отдельном потоке Создать задачу PhysicsSystem -> Update
		//Запуск графики в отдельном потоке
	}
	catch (std::exception& e)
	{
		Diagnostics::Logger::Get().SendMessage("(Core) Error: " + std::string(e.what()) + ".", Diagnostics::MessageType::Error);
		m_isRunning = false;
	}
	m_isRunning = false;
}

EngineCore::EngineCore()
{
	Init();
}

void EngineCore::Init()
{
	Diagnostics::Logger::Get().SendMessage("(Core) Core initialization has started.", Diagnostics::MessageType::Info);

	//System registration
	ServiceLocator::Set<Windows::WindowsManager, Windows::WindowsManager>();
	ServiceLocator::Set<Input::InputSystem, Input::InputSystem>();
	Diagnostics::Logger::Get().SendMessage("(Core) Core initialization has ended.", Diagnostics::MessageType::Info);
}
