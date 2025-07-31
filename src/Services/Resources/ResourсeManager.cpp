#include <Services/Resources/ResourceManager.hpp>





void ResourceManager::Init() 
{
	if (m_isValid) return; 
	
	m_isValid = true;
}

void ResourceManager::Shutdown()
{
	m_isValid = false;
	Clear();
}

void ResourceManager::Clear()
{
    m_resources.clear();
}

