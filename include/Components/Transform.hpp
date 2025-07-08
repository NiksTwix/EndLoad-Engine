#pragma once
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <ECS\ECS.hpp>
#include <vector>




struct Transform 
{
	bool Visible = true;
	Math::Vector3 Translation;
	Math::Vector3 Scale = Math::Vector3(1,1,1);
	Math::Quaternion RotationQuat;
};


class SceneContext;


class TransformService : public ECS::IComponentService
{
private:
	static void MergeTransforms(Transform& child, const Transform& parent);

public:
	TransformService();

	virtual void Init() override;  // Опционально
	virtual void Shutdown() override;

	Transform GetGlobalTransform(ECS::EntityID id,  SceneContext& context);

	void Translate(Transform& translated, Math::Vector3 translation);
	void Scale(Transform& scaled, Math::Vector3 scale);
	void RotateLocal(Transform& rotated, Math::Vector3 axis, float value);
	void RotateGlobal(Transform& rotated, Math::Vector3 axis, float value);
	Math::Matrix4x4 GetModelMatrix(Transform& transform);

	void Update(ECS::EntitySpace* eSpace) override;
	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;
};