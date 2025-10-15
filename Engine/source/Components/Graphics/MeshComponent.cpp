#include <Components/Graphics/MeshComponent.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>


namespace Components 
{
	void MeshComponentService::Init()
	{
	}
	void MeshComponentService::Update(ECS::EntitySpace* eSpace)
	{
		m_eSpace = eSpace;
	}
	void MeshComponentService::Shutdown()
	{
	}

	void MeshComponentService::SetResourceData(MeshComponent& component, Resources::ResourceID new_resource) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
		if (!rm) return; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return; //TODO error

		auto res = resourceFrame->GetResource<Resources::MeshResource>(new_resource);
		if (!res) 
		{
			component.previousResource = component.currentResource;
			component.meshID = Definitions::InvalidID;
			component.isValid = false;
			component.currentResource = Definitions::InvalidID;
			return;
		}

		component.previousResource = component.currentResource;
		component.currentResource = new_resource;
		component.meshID = res->GetMeshID();
		//Проверка mesh id также будет meshrendermodule
	}

	bool MeshComponentService::InitResource(MeshComponent& component) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
		if (!rm) return false; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return false; //TODO error

		auto res = resourceFrame->GetResource<Resources::MeshResource>(component.currentResource);
		if (!res)
		{
			component.previousResource = component.currentResource;
			component.meshID = Definitions::InvalidID;
			component.isValid = false;
			component.currentResource = Definitions::InvalidID;
			return false;
		}
		return res->Init();
	}

}