#include <Systems\Graphics\Windows\WindowsManager.hpp>



namespace Windows 
{
	void WindowsManager::HandleClosedWindows()
	{
		auto* resource_manager = Core::ServiceLocator::Get<Resources::ResourceManager>();

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
				if (resource_manager) resource_manager->ClearWindowData(id);
			}
			m_closedWindows.clear();
		}
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