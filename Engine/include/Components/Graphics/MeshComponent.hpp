#pragma once
#include <Components/IRComponentService.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <Resources/MeshResource.hpp>
namespace Components
{
	struct MeshComponent
	{
		Graphics::MeshID meshID;
		Resources::ResourceID resource = Definitions::InvalidID;
		bool isValid;
	};
	class MeshComponentService : public IRComponentService
	{
	public:
		void Init() override;
		void Update(ECS::EntitySpace* eSpace) override;
		void Shutdown() override;

		void SetResourceData(MeshComponent& component, Resources::ResourceID new_resource) const;
		bool UpdateResourceState(ECS::EntityID entity) override;
	};
}