#pragma once
#include <ELECS\ECS.hpp>
#include <ELMath/include/MathFunctions.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <Resources/MeshResource.hpp>
namespace Components
{
	struct MeshComponent
	{
		Graphics::MeshID meshID;
		Resources::ResourceID currentResource = Definitions::InvalidID;
		Resources::ResourceID previousResource = Definitions::InvalidID;
		bool isValid;
	};
	class MeshComponentService : public ECS::IComponentService
	{
	public:
		void Init() override;
		void Update(ECS::EntitySpace* eSpace) override;
		void Shutdown() override;

		void SetResourceData(MeshComponent& component, Resources::ResourceID new_resource) const;

		bool InitResource(MeshComponent& component) const;

	};
}