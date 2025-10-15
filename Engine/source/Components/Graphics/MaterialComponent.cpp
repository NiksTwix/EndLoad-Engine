#include <Components/Graphics/MaterialComponent.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>


namespace Components
{
	void MaterialComponentService::Init()
	{
	}
	void MaterialComponentService::Update(ECS::EntitySpace* eSpace)
	{
		m_eSpace = eSpace;
	}
	void MaterialComponentService::Shutdown()
	{
	}

	void MaterialComponentService::SetResourceShaderData(MaterialComponent& component, Resources::ResourceID shader_resource) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
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

	void MaterialComponentService::SetResourceMaterialData(MaterialComponent& component, Resources::ResourceID material_resource) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
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
	}

	bool MaterialComponentService::InitResources(MaterialComponent& component) const	//Also check validity
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
		if (!rm) return; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return; //TODO error

		//Reset state
		component.shaderID = Definitions::InvalidID;

		auto res = resourceFrame->GetResource<Resources::MaterialResource>(component.materialResource);
		if (!res)
		{
			component.materialResource = Definitions::InvalidID;
			component.texturesInited = false;
			return false;
		}
		if (!component.texturesInited) 
		{
			const Graphics::MaterialData& data = res->GetData();

			if (data.textureMode == "path") 
			{
				//Loading textures
			}
			else if (data.textureMode == "base64") 
			{
				//Create and initialize Texture resources, sets binary data and attach it to frame
			}
			//Saving Texture ID (gpu handle) to uniforms

			component.texturesInited = true;
		}
		auto res1 = resourceFrame->GetResource<Resources::ShaderResource>(component.shaderResource);
		if (!res1)
		{
			component.shaderResource = Definitions::InvalidID;
			component.shaderID = Definitions::InvalidID;
			return false;
		}

		component.shaderID = res1->GetShaderID();

		return true;
	}
}