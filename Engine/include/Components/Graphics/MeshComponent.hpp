#pragma once
#include <Components/IRComponentService.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <Resources/MeshResource.hpp>
namespace Components
{
	struct AABB
	{
		Math::Vector3 Max;
		Math::Vector3 Min;

		Math::Vector3 Center;
		Math::Vector3 Size;
	};
	struct MeshComponent
	{
		Graphics::MeshID meshID;
		Resources::ResourceID resource = Definitions::InvalidID;
		bool isValid;
		AABB aabb;
		AABB aabbWorld;
	};
	class MeshComponentService : public IRComponentService
	{
	public:
		void Init() override;
		void Update(ECS::EntitySpace* eSpace) override;
		void Shutdown() override;

		void SetResourceData(MeshComponent& component, Resources::ResourceID new_resource) const;
		bool UpdateResourceState(ECS::EntityID entity) override;

		void BuildAABB(MeshComponent& component);
		void UpdateWorldAABB(MeshComponent& component, const Math::Matrix4x4& modelMatrix);
	};
}