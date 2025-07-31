#pragma once
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <ECS\ECS.hpp>
#include <vector>


struct LocalTransformComponent 
{
	bool Visible = true;
	Math::Vector3 Translation;
	Math::Vector3 Scale = Math::Vector3(1,1,1);
	Math::Quaternion RotationQuat;
	bool is_dirty = true;
};
struct GlobalTransformComponent  //read - only
{
	Math::Matrix4x4 ModelMatrix;
	Math::Vector3 Translation;
	Math::Vector3 Scale = Math::Vector3(1, 1, 1);
	Math::Quaternion RotationQuat;
	bool Visible = true;
	//std::atomic<bool> is_valid{ false };
};

class SceneContext;


class TransformService : public ECS::IComponentService
{
private:
	static void MergeTransforms(LocalTransformComponent& child, const LocalTransformComponent& parent);

	void MarkDirty(ECS::EntityID id);

public:
	TransformService();

	virtual void Init() override;  // Опционально
	virtual void Shutdown() override;

	void UpdateSingleGlobalTransform(ECS::EntityID id,  SceneContext& context);


	void UpdateGlobalTransforms();


	void Translate(ECS::EntityID entity, Math::Vector3 translation);
	void Scale(ECS::EntityID entity, Math::Vector3 scale);
	void RotateLocal(ECS::EntityID entity, Math::Vector3 axis, float value);
	void RotateGlobal(ECS::EntityID entity, Math::Vector3 axis, float value);

	Math::Matrix4x4 GetModelMatrix(LocalTransformComponent& transform);
	Math::Matrix4x4 GetModelMatrix(GlobalTransformComponent& transform);

	void Update(ECS::EntitySpace* eSpace) override;



	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;
};

