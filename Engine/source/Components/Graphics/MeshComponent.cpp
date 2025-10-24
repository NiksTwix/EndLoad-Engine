#include <Components/Graphics/MeshComponent.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourcesManager.hpp>


namespace Components 
{
	MeshComponentService::MeshComponentService()
	{
		m_stringType = "Mesh";
		m_ComponentType = typeid(MeshComponent);
	}

	void MeshComponentService::SetResourceData(MeshComponent& component, Resources::ResourceID new_resource) const
	{
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
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
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
		MeshComponent& component = m_eSpace->GetComponent<MeshComponent>(entity);
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

		component.meshID = res->GetMeshID();

		component.isValid = true;
		return res->Init();	//If has already inited returns true
	}

	void MeshComponentService::UpdateWorldAABB(MeshComponent& component, const Math::Matrix4x4& modelMatrix)
	{
		//OBB
		std::vector<Math::Vector3> corners =
		{
			Math::Vector3(component.aabb.Max.x, component.aabb.Max.y,component.aabb.Max.z),
			Math::Vector3(component.aabb.Min.x, component.aabb.Max.y,component.aabb.Max.z),
			Math::Vector3(component.aabb.Min.x, component.aabb.Min.y,component.aabb.Max.z),
			Math::Vector3(component.aabb.Min.x, component.aabb.Min.y,component.aabb.Min.z),

			Math::Vector3(component.aabb.Max.x, component.aabb.Max.y,component.aabb.Min.z),
			Math::Vector3(component.aabb.Max.x, component.aabb.Min.y,component.aabb.Min.z),
			Math::Vector3(component.aabb.Max.x, component.aabb.Min.y,component.aabb.Max.z),
			Math::Vector3(component.aabb.Min.x, component.aabb.Max.y,component.aabb.Min.z),
		};

		Math::Vector3 worldMin(FLT_MAX), worldMax(-FLT_MAX);
		for (const auto& corner : corners) {
			Math::Vector3 worldPos = (Math::Vector3)(modelMatrix * Math::Vector4(corner.x, corner.y, corner.z, 1.0f));
			worldMin = Math::Min(worldMin, worldPos);
			worldMax = Math::Max(worldMax, worldPos);
		}

		AABB obb(worldMax, worldMin, (worldMin + worldMax) * 0.5f, worldMax - worldMin);

		component.aabbWorld = obb;
	}
	void MeshComponentService::BuildAABB(MeshComponent& component)
	{
		auto* frame = Core::ServiceLocator::Get<Resources::ResourcesManager>()->GetActiveFrame();

		if (!frame) 
		{
			//TODO LOGGING
			return;
		}
		auto resource = frame->GetResource<Resources::MeshResource>(component.resource);
		if (!resource) 
		{
			//TODO LOGGING
			return;
		}

		AABB aabb;

		Math::Vector3 MaxAABB(-FLT_MAX);
		Math::Vector3 MinAABB(FLT_MAX);

		for (auto vertex : resource->GetMeshData().vertices) {
			if (vertex.position.x > MaxAABB.x) MaxAABB.x = vertex.position.x;
			if (vertex.position.y > MaxAABB.y) MaxAABB.y = vertex.position.y;
			if (vertex.position.z > MaxAABB.z) MaxAABB.z = vertex.position.z;

			if (vertex.position.x < MinAABB.x) MinAABB.x = vertex.position.x;
			if (vertex.position.y < MinAABB.y) MinAABB.y = vertex.position.y;
			if (vertex.position.z < MinAABB.z) MinAABB.z = vertex.position.z;
		}

		aabb.Max = MaxAABB;
		aabb.Min = MinAABB;

		aabb.Size = aabb.Max - aabb.Min;

		aabb.Center = (aabb.Min + aabb.Max) * 0.5f;

		component.aabb = aabb;
	}
}