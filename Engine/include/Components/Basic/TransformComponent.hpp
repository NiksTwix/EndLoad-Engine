#pragma once
#include <ELMath\include\Basis.hpp>
#include <ELMath\include\MathFunctions.hpp>
#include <Components/ELComponentService.hpp>

namespace Scenes 
{
	class SceneContext;
}

namespace Components
{
	struct LocalTransformComponent
	{
		Math::Basis basis;	//Rotation and scale
		Math::Vector3 origin;
		bool visible;
		bool is_dirty = true;
	};
	struct GlobalTransformComponent
	{
		Math::Matrix4x4 modelMatrix;
		Math::Basis basis;	//Rotation and scale
		Math::Vector3 origin;
		bool visible;
		bool is_dirty = true;
	};


	class TransformComponentService : public ELComponentService
	{
	private:
		static void MergeTransforms(LocalTransformComponent& child, const LocalTransformComponent& parent);

		void MarkDirty(ECS::EntityID id);

	public:
		TransformComponentService();

		void UpdateSingleGlobalTransform(ECS::EntityID id, Scenes::SceneContext& context);
		void UpdateGlobalTransforms();
		void Translate(ECS::EntityID entity, Math::Vector3 translation);
		void TranslateLocal(ECS::EntityID entity, Math::Vector3 translation);
		void Scale(ECS::EntityID entity, Math::Vector3 scale);
		void RotateLocal(ECS::EntityID entity, Math::Vector3 axis, float value);
		void RotateGlobal(ECS::EntityID entity, Math::Vector3 axis, float value);
		Math::Matrix4x4 GetModelMatrix(LocalTransformComponent& transform);
		Math::Matrix4x4 GetModelMatrix(GlobalTransformComponent& transform);
		//std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
		//void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;
	};
}