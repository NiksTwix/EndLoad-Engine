#include <Services\ResourcesManager\ResourceManager.hpp>


namespace Resources
{

	void ResourceManager::Shutdown()
	{
		for (auto& k:m_resources) 
		{
			k.second.ClearResources();
		}
		m_resources.clear();
		m_isValid = false;
	}


	void ResourceManager::SetActiveWindow(Windows::WindowID new_active_window)
	{
		m_activeWindow = new_active_window;
	}

	void ResourceManager::ClearWindowData(Windows::WindowID id)
	{
		if (m_resources.count(id))
		{
			m_resources[id].ClearResources();
			m_resources.erase(id);
		}
	}

	void ResourceManager::ClearWindowCache(Windows::WindowID id)
	{
		if (m_resources.count(id))
		{
			m_resources[id].CleanupUnusedResources();
		}
	}

	void ResourceManager::Init()
	{
		if (m_isValid) return; m_isValid = true;
	}
	void ResourcesFrame::DeleteResource(ResourceID id, bool is_runtime)
	{
		if (id == Definitions::InvalidID) return;
		if (is_runtime)
		{
			if (!m_runTimeResources.count(id)) return;
			m_runTimeResources.erase(id);
			m_IdStr.erase(id);
		}
		else
		{
			if (!m_resources.count(id)) return;
			m_resources.erase(id);
			m_IdStr.erase(id);
		}

	}
	void ResourcesFrame::ClearResources()
	{
		for (auto& [path, resource] : m_resources)
		{
			resource.m_resource->Uninit();
			resource.m_resource->Release();
			Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Resource \"" + std::to_string(path) + "\" has released. Window id: " + std::to_string(m_ownerWindow) + ".");
		}
		for (auto& [name, resource] : m_runTimeResources)
		{
			resource.m_resource->Uninit();
			resource.m_resource->Release();
			Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Runtime resource \"" + std::to_string(name) + "\" has released. Window id: " + std::to_string(m_ownerWindow) + ".");
		}
		m_resources.clear();
		m_runTimeResources.clear();
		m_IdStr.clear();
		m_strId.clear();
	}
	bool ResourcesFrame::Exists(const std::string& key, bool is_runtime) const
	{
		return m_strId.count(key) ? Exists(m_strId.at(key), is_runtime) : false;
	}
	bool ResourcesFrame::Exists(ResourceID id, bool is_runtime = false) const
	{
		if (is_runtime) return m_runTimeResources.count(id);
		return m_resources.count(id);
	}
	void ResourcesFrame::InitAll()
	{
		for (auto& [path, resource] : m_resources)
		{
			if (resource.m_resource->GetState() == ResourceState::Loaded)resource.m_resource->Init();
			if (resource.m_resource->GetState() == ResourceState::Initialized)Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Resource \"" + std::to_string(path) + "\" has initialized. Window id: " + std::to_string(m_ownerWindow) + ".");
		}
		for (auto& [name, resource] : m_runTimeResources)
		{
			if (resource.m_resource->GetState() == ResourceState::Loaded)resource.m_resource->Init();
			if (resource.m_resource->GetState() == ResourceState::Initialized)Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Runtime resource \"" + std::to_string(name) + "\" has initialized. Window id: " + std::to_string(m_ownerWindow) + ".");
		}
	}
	void ResourcesFrame::CleanupUnusedResources()
	{
		auto now = std::chrono::steady_clock::now();
		std::vector<ResourceID> toRemove;
		for (auto& [path, cell] : m_resources) {
			auto timeSinceLastUse = now - cell.m_lastUsedTime;
			if (!cell.m_isPinned &&
				timeSinceLastUse > MaxUnusedPeriod && cell.m_useCount < MinUsedCounts) toRemove.push_back(path);
		}
		for (auto& path : toRemove) DeleteResource(path);
	}
}

