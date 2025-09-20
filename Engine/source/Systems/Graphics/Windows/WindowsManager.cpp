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