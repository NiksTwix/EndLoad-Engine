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


	void ResourceManager::Init()
	{
		if (m_isValid) return; m_isValid = true;
	}
}

