#include <Resources/MaterialResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>

namespace Resources 
{
	bool MaterialResource::SetData(std::any data)
	{
		// Type-safe установка данных
		if (!ValidateData<Graphics::MaterialData>(data)) return false;
		m_data = std::any_cast<Graphics::MaterialData>(data);
		m_state = ResourceState::Loaded;
		return true;
	}
	bool MaterialResource::Init()
	{
		m_state = ResourceState::Initialized;
		return true;
	}
	bool MaterialResource::Uninit()
	{
		m_state = ResourceState::Loaded;
		return true;
	}
	bool MaterialResource::Load(const std::string& path)
	{
		return false;
	}
	std::type_index MaterialResource::GetDataType() const
	{
		return typeid(Graphics::MaterialData);
	}
	void MaterialResource::Release()
	{
		m_data = Graphics::MaterialData();
		m_state = ResourceState::NotLoaded;
	}
	std::string MaterialResource::GetType() const
	{
		return "Material";
	}
}