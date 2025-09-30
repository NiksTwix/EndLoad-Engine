#include <Services\ResourcesManager\ResourceManager.hpp>


namespace Resources
{

	void ResourceManager::Shutdown()
	{
		for (auto& k:m_resources) 
		{
			k.second->ClearAllResources();
		}
		m_resources.clear();
		m_isValid = false;
	}

	void ResourceManager::AttachStaticResources(const std::vector<std::shared_ptr<Resources::IResource>>& resources)
	{
		if (m_activeWindow == Definitions::InvalidID) 
		{
			Diagnostics::Logger::Get().SendMessage("(ResourceManager) Invalid active window.");
			return;
		}

		for (auto resource : resources) 
		{
			m_resources[m_activeWindow]->AddStaticResource(resource);
		}
	}


	ResourceID ResourceManager::AttachResourceToFrame(std::shared_ptr<IResource> resource)
	{
		auto frame = GetActiveFrame();
		if (!frame)
		{
			Diagnostics::Logger::Get().SendMessage("(ResourceManager) Attaching of resource failed: active frame is invalid.", Diagnostics::MessageType::Error);
			return;
		}
		return frame->AddStaticResource(resource);
	}

	ResourcesFrame* ResourceManager::GetActiveFrame()
	{
		if (m_activeWindow == Definitions::InvalidID) return nullptr;
		return m_resources.count(m_activeWindow) ? m_resources.at(m_activeWindow).get() : nullptr;
	}

	void ResourceManager::SetActiveWindow(Windows::WindowID new_active_window)
	{
		m_activeWindow = new_active_window;
		if (m_resources.count(m_activeWindow) == 0 && m_activeWindow != Definitions::InvalidID) 
		{
			m_resources[m_activeWindow] = std::make_shared<ResourcesFrame>(m_activeWindow);
		}
	}

	void ResourceManager::ClearWindowData(Windows::WindowID id)
	{
		if (m_resources.count(id))
		{
			m_resources[id]->ClearAllResources();
			m_resources.erase(id);
		}
	}

	void ResourceManager::ClearWindowCache(Windows::WindowID id)
	{
		if (m_resources.count(id))
		{
			m_resources[id]->CleanupUnusedResources();
		}
	}

	void ResourceManager::Init()
	{
		if (m_isValid) return; m_isValid = true;
	}
	bool ResourcesFrame::Exists(ResourceID id)
	{
		return GetStaticResource(id) != nullptr;
	}
	bool ResourcesFrame::Exists(const std::string& name)
	{
		return GetRuntimeResource(name) != nullptr;
	}
	void ResourcesFrame::DeleteResource(ResourceID id)
	{
		if (!m_staticResources.count(id)) return;
		m_staticResources[id].m_resource->Uninit();
		m_staticResources.erase(id);
	}
	void ResourcesFrame::DeleteResource(const std::string& name)
	{
		if (!m_runtimeResources.count(name)) return;
		m_runtimeResources[name].m_resource->Uninit();
		m_runtimeResources.erase(name);
	}
	void ResourcesFrame::ClearStaticResources() {
		for (auto& [id, cell] : m_staticResources) {
			cell.m_resource->Uninit();
			Diagnostics::Logger::Get().SendMessage(
				"(ResourceFrame) Static resource released. Window: " +
				std::to_string(m_ownerWindow)
			);
		}
		m_staticResources.clear();
	}

	void ResourcesFrame::ClearRuntimeResources() {
		for (auto& [name, cell] : m_runtimeResources) {
			cell.m_resource->Uninit();
			Diagnostics::Logger::Get().SendMessage(
				"(ResourceFrame) Runtime resource \"" + name + "\" released. Window: " +
				std::to_string(m_ownerWindow)
			);
		}
		m_runtimeResources.clear();
	}

	void ResourcesFrame::ClearAllResources() {
		ClearStaticResources();
		ClearRuntimeResources();
	}

	void ResourcesFrame::CleanupUnusedResources()
	{
		auto now = std::chrono::steady_clock::now();
		std::vector<ResourceID> toRemove;
		for (auto& [path, cell] : m_staticResources) {
			auto timeSinceLastUse = now - cell.m_lastUsedTime;
			if (!cell.m_isPinned &&
				timeSinceLastUse > MaxUnusedPeriod && cell.m_useCount < MinUsedCounts) toRemove.push_back(path);
		}
		for (auto& path : toRemove) DeleteResource(path);
	}
}

