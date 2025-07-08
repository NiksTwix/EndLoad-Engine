#include <Components\Transform.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>

TransformService::TransformService()
{
	m_stringType = "Transform";
	m_ComponentType = typeid(Transform);
}

void TransformService::Init()
{

}

void TransformService::Shutdown()
{
}

Transform TransformService::GetGlobalTransform(ECS::EntityID id, SceneContext& context)
{
	Transform global = Transform(m_eSpace->GetComponent<Transform>(id));	//Копирование? Надеюсь
	ECS::EntityID current = id;
	auto& tree = context.GetEntitySpace().GetTree();
	auto& registry = context.GetEntitySpace().GetRegistry();
	
	while (current != ECS::INVALID_ID && current != tree.GetRoot()->id) {
		ECS::EntityID parent = tree.GetParent(current);
		if (parent == ECS::INVALID_ID) break;

		if (const Transform parentTransform = registry.Get<Transform>(parent); registry.Has<Transform>(parent)) {
			MergeTransforms(global, parentTransform);
		}
		current = parent;
	}

	return global;
} 

void TransformService::Translate(Transform& translated, Math::Vector3 translation)
{
	translated.Translation += translation;
	//TODO isDirty
}

void TransformService::Scale(Transform& scaled, Math::Vector3 scale)
{
	scaled.Scale *= scale;
}

void TransformService::RotateLocal(Transform& rotated, Math::Vector3 axis, float angleRadians) {
	if (Math::LengthSquared(axis) < FLT_EPSILON) return;

	auto normalized = Math::Normalize(axis);
	Math::Quaternion deltaRot = Math::FromAxisAngle(normalized, angleRadians);

	rotated.RotationQuat = Math::Normalize(rotated.RotationQuat * deltaRot);
}

void TransformService::RotateGlobal(Transform& rotated, Math::Vector3 axis, float angleRadians) {
	if (Math::LengthSquared(axis) < FLT_EPSILON) return;

	auto normalized = Math::Normalize(axis);
	Math::Quaternion deltaRot = Math::FromAxisAngle(normalized, angleRadians);

	rotated.RotationQuat = Math::Normalize(deltaRot * rotated.RotationQuat);
}

Math::Matrix4x4 TransformService::GetModelMatrix(Transform& transform)
{
	auto translation_mat = Math::Translate(Math::Matrix4x4(), transform.Translation);
	auto scale_mat = Math::Scale(Math::Matrix4x4(), transform.Scale);

	auto rotation_mat = Math::ToMatrix4x4(transform.RotationQuat);

	return translation_mat * rotation_mat * scale_mat;
}

void TransformService::MergeTransforms(Transform& child, const Transform& parent)	//Видимость также меняется. Видимость ребёнка зависит от видимости родителя
{
	child.RotationQuat = Math::Normalize(child.RotationQuat * parent.RotationQuat); // Умножение кватернионов! Старое на новое
	child.Translation = parent.Translation + (parent.RotationQuat * child.Translation) * parent.Scale;
	child.Scale = child.Scale * parent.Scale;
	child.Visible = parent.Visible && child.Visible; // Логическое И вместо умножения
}



void TransformService::Update(ECS::EntitySpace* eSpace)
{
	m_eSpace = eSpace;
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> TransformService::GetComponentFields(void* component)
{
	std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
	if (component == nullptr) return result;
	Transform* transform = static_cast<Transform*>(component);

	result["Translation"] = std::vector<float>({ transform->Translation.x, transform->Translation.y, transform->Translation.z });
	result["Scale"] = std::vector<float>({ transform->Scale.x, transform->Scale.y, transform->Scale.z });
	result["Rotation"] = std::vector<float>({ transform->RotationQuat.x, transform->RotationQuat.y, transform->RotationQuat.z, transform->RotationQuat.w });
	result["Visible"] = transform->Visible;
	return result;
}

void TransformService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
	Transform transform;

	for (auto [fieldName, fieldValue] : fields)
	{
		if (fieldName == "Visible") {
			if (auto wu = ECS::ESDL::ValueParser::GetAs<bool>(fieldValue)) {
				transform.Visible = *wu;
			}
		}
		else if (fieldName == "Translation") 
		{
			if (auto wu = ECS::ESDL::ValueParser::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 3)transform.Translation = Math::Vector3(wu->at(0), wu->at(1), wu->at(2));
			}
		}
		else if (fieldName == "Scale")
		{
			if (auto wu = ECS::ESDL::ValueParser::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 3)transform.Scale = Math::Vector3(wu->at(0), wu->at(1), wu->at(2));
			}
		}
		else if (fieldName == "Rotation")
		{
			if (auto wu = ECS::ESDL::ValueParser::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 4) transform.RotationQuat = Math::Quaternion(wu->at(0), wu->at(1), wu->at(2), wu->at(3));
			}
		}
	}
	if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<Transform>(id, transform);
	else
	{
		ServiceLocator::Get<Logger>()->Log("Transform's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
	}
}
