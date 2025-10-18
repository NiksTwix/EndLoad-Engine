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
			component.resource = Definitions::InvalidID;
			component.meshID = Definitions::InvalidID;
			component.isValid = false;
			return;
		}

		component.resource = new_resource;
		component.meshID = res->GetMeshID();
		//Проверка mesh id также будет meshrendermodule
	}

	bool MeshComponentService::UpdateResourceState(ECS::EntityID entity)
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
		MeshComponent component = m_eSpace->GetComponent<MeshComponent>(entity);
		if (!rm) return false; // TODO error
		auto resourceFrame = rm->GetActiveFrame();
		if (!resourceFrame) return false; //TODO error

		auto res = resourceFrame->GetResource<Resources::MeshResource>(component.resource);	//Update deleting timer
		if (!res)
		{
			component.resource = Definitions::InvalidID;
			component.meshID = Definitions::InvalidID;
			component.isValid = false;
			return false;
		}
		component.isValid = true;
		return res->Init();
	}

}