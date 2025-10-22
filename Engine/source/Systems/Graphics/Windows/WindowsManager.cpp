#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Systems\Input\InputSystem.hpp>


namespace Windows 
{
	void WindowsManager::HandleClosedWindows()
	{
		auto* resource_manager = Core::ServiceLocator::Get<Resources::ResourcesManager>();

		for (auto& [id, window] : m_windows)
		{
			if (window->IsValid())window->Update();
			if (window->IsClosed()) m_closedWindows.push_back(id);
		}

		if (m_closedWindows.size() > 0)
		{
			for (auto id : m_closedWindows)
			{
				m_windows.erase(id);
				RemoveInputFrame(id);
				if (resource_manager) resource_manager->ClearWindowData(id);
			}
			m_closedWindows.clear();
		}
	}
	void WindowsManager::RemoveInputFrame(WindowID window)
	{
		auto* input = Core::ServiceLocator::Get<Input::InputSystem>();
		if (!input) 
		{
			Diagnostics::Logger::Get().SendMessage("(WindowsManager) IF's removing failed: InputSystem is invalid.", Diagnostics::MessageType::Error);
			return;
		}

		input->RemoveFrame(window);
	}
	void WindowsManager::SetRenderWindow(WindowID windowId)
	{
		if (!m_windows.count(windowId) || m_windows[windowId]->IsClosed())
		{
			Diagnostics::Logger::Get().SendMessage("(WindowsManager) Invalid render window with id " + std::to_string(windowId) + ".", Diagnostics::MessageType::Error);
			m_renderWindow = nullptr;
			return;
		}
		m_renderWindow = m_windows[windowId].get();
		Core::ServiceLocator::Get<Resources::ResourcesManager>()->SetActiveWindow(windowId);
		m_windows[windowId]->MakeCurrent();
	}
	void WindowsManager::Update()
	{
		HandleClosedWindows();
	}
	void WindowsManager::Shutdown()
	{
		for (auto& [id, window] : m_windows)
		{
			window->Close();
		}
		HandleClosedWindows();
		m_windows.clear();
	}

}