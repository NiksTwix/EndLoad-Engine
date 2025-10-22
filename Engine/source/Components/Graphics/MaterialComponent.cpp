#include <Components/Graphics/MaterialComponent.hpp>
#include <Resources/TextureResource.hpp>

namespace Components
{
	MaterialComponentService::MaterialComponentService()
	{
		m_stringType = "Material";
		m_ComponentType = typeid(MaterialComponent);
	}

	void MaterialComponentService::SetResourceShaderData(MaterialComponent& component, Resources::ResourceID shader_resource) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
		if (!rm) return; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return; //TODO error

		auto res = resourceFrame->GetResource<Resources::ShaderResource>(shader_resource);
		if (!res)
		{
			component.shaderResource = shader_resource;
			component.shaderID = Definitions::InvalidID;
			component.isValid = false;
			return;
		}

		component.shaderID = res->GetShaderID();
	}

	void MaterialComponentService::SetResourceMaterialData(MaterialComponent& component, Resources::ResourceID material_resource, bool generate_textures) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
		if (!rm) return; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return; //TODO error

		auto res = resourceFrame->GetResource<Resources::MaterialResource>(material_resource);
		if (!res)
		{
			component.materialResource = Definitions::InvalidID;
			return;
		}

		component.materialResource = material_resource;

		if (!generate_textures) return;

		for (auto& texture : res->GetData().raw_textures)	//Type - texture data
		{
			auto res1 = std::make_shared<Resources::TextureResource>();
			res1->SetData(texture.second);
			component.textures[texture.first] = resourceFrame->AddStaticResource(res1);
		}

	}
	bool MaterialComponentService::UpdateResourceState(ECS::EntityID entity)
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();

		MaterialComponent component = m_eSpace->GetComponent<MaterialComponent>(entity);

		if (!rm) return false; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return false; //TODO error

		//Reset state
		component.shaderID = Definitions::InvalidID;

		auto res = resourceFrame->GetResource<Resources::MaterialResource>(component.materialResource);
		if (!res)
		{
			component.materialResource = Definitions::InvalidID;
			return false;
		}
		auto res1 = resourceFrame->GetResource<Resources::ShaderResource>(component.shaderResource);
		if (!res1)
		{
			component.shaderResource = Definitions::InvalidID;
			component.shaderID = Definitions::InvalidID;
			return false;
		}
		if (!res1->Init()) return false;	//If inited  returns true
		component.shaderID = res1->GetShaderID();
		//checking textures

		for (auto& resource : component.textures)
		{
			if (resource.second == Definitions::InvalidID) continue;
			auto res2 = resourceFrame->GetResource<Resources::TextureResource>(resource.second);
			if (!res2)
			{
				resource.second = Definitions::InvalidID;
				component.texturesID[resource.first] = Definitions::InvalidID;
				continue;
			}
			if (res2->Init())component.texturesID[resource.first] = res2->GetDataID();
			
		}
		return true;
	}
}