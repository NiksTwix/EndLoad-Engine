#include <queue>
#include <Components\Basic\TransformComponent.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Components\ComponentTags.hpp>

TransformService::TransformService()
{
	m_stringType = "Transform";
	m_ComponentType = typeid(LocalTransformComponent);
}

void TransformService::Init()
{

}

void TransformService::Shutdown()
{
}

void TransformService::UpdateSingleGlobalTransform(ECS::EntityID id, SceneContext& context)
{
	LocalTransformComponent local = LocalTransformComponent(m_eSpace->GetComponent<LocalTransformComponent>(id));
	ECS::EntityID current = id;
	auto& tree = context.GetEntitySpace().GetTree();
	auto& registry = context.GetEntitySpace().GetRegistry();
	
	while (current != ECS::INVALID_ID && current != tree.GetRoot()->id) {
		ECS::EntityID parent = tree.GetParent(current);
		if (parent == ECS::INVALID_ID) break;

		if (const LocalTransformComponent parentTransform = registry.Get<LocalTransformComponent>(parent); registry.Has<LocalTransformComponent>(parent)) {
			MergeTransforms(local, parentTransform);
		}
		current = parent;
	}

	GlobalTransformComponent gt;

	gt.Visible = local.Visible;
	gt.Translation = local.Translation;
	gt.RotationQuat = local.RotationQuat;
	gt.Scale = local.Scale;
	gt.ModelMatrix = GetModelMatrix(gt);

	registry.Replace<GlobalTransformComponent>(id, gt);
}

void TransformService::UpdateGlobalTransforms()
{
	if (m_eSpace == nullptr) return;

	ECS::Registry& registry = m_eSpace->GetRegistry();
	ECS::HierarchyTree& tree = m_eSpace->GetTree();
	ECS::ComponentServiceLocator& csl = m_eSpace->GetServiceLocator();

	std::queue<ECS::EntityID> processQueue;
	//Предподготовка
	for (auto child : tree.GetRootChildren())
	{
		auto* t = registry.TryGet<LocalTransformComponent>(child);
		if (t != nullptr && t->is_dirty)
		{
			GlobalTransformComponent gt;
			gt.RotationQuat = t->RotationQuat;
			gt.Translation = t->Translation;
			gt.Scale = t->Scale;
			gt.Visible = t->Visible;
			gt.ModelMatrix = GetModelMatrix(gt);
			registry.Replace<GlobalTransformComponent>(child, gt);
			processQueue.push(child);
			t->is_dirty = false;
		}
		else if (t != nullptr && !t->is_dirty)
		{
			processQueue.push(child);
		}
	}
	//Основной цикл
	while (!processQueue.empty())
	{
		ECS::EntityID current = processQueue.front();
		processQueue.pop();

		auto parent = registry.TryGet<GlobalTransformComponent>(current);

		for (auto child : tree.GetChildren(current))
		{
			auto* t = registry.TryGet<LocalTransformComponent>(child);

			if (t != nullptr && t->is_dirty)
			{
				GlobalTransformComponent gt;
				gt.RotationQuat = Math::Normalize(t->RotationQuat * parent->RotationQuat); // Умножение кватернионов! Старое на новое
				gt.Translation = parent->Translation + (parent->RotationQuat * t->Translation) * parent->Scale;
				gt.Scale = t->Scale * parent->Scale;
				gt.Visible = parent->Visible && t->Visible;
				gt.ModelMatrix = GetModelMatrix(gt);
				registry.Replace<GlobalTransformComponent>(child, gt);
				processQueue.push(child);
				t->is_dirty = false;
			}
			else if (t != nullptr && !t->is_dirty) 
			{
				processQueue.push(child);
			}
		}
	}
}


void TransformService::Translate(ECS::EntityID entity, Math::Vector3 translation)
{
	auto& translated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
	translated.Translation += translation;
	//TODO isDirty
	MarkDirty(entity);
}

void TransformService::Scale(ECS::EntityID entity, Math::Vector3 scale)
{
	auto& scaled = m_eSpace->GetComponent<LocalTransformComponent>(entity);
	scaled.Scale *= scale;
	MarkDirty(entity);
}

void TransformService::RotateLocal(ECS::EntityID entity, Math::Vector3 axis, float angleRadians) {
	if (Math::LengthSquared(axis) < FLT_EPSILON) return;
	auto& rotated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
	auto normalized = Math::Normalize(axis);
	Math::Quaternion deltaRot = Math::FromAxisAngle(normalized, angleRadians);

	rotated.RotationQuat = Math::Normalize(rotated.RotationQuat * deltaRot);
	MarkDirty(entity);
}

void TransformService::RotateGlobal(ECS::EntityID entity, Math::Vector3 axis, float angleRadians) {
	if (Math::LengthSquared(axis) < FLT_EPSILON) return;
	auto& rotated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
	auto normalized = Math::Normalize(axis);
	Math::Quaternion deltaRot = Math::FromAxisAngle(normalized, angleRadians);

	rotated.RotationQuat = Math::Normalize(deltaRot * rotated.RotationQuat);
	MarkDirty(entity);
}

Math::Matrix4x4 TransformService::GetModelMatrix(LocalTransformComponent& transform)
{
	auto translation_mat = Math::Translate(Math::Matrix4x4(), transform.Translation);
	auto scale_mat = Math::Scale(Math::Matrix4x4(), transform.Scale);

	auto rotation_mat = Math::ToMatrix4x4(transform.RotationQuat);

	return translation_mat * rotation_mat * scale_mat;
}

Math::Matrix4x4 TransformService::GetModelMatrix(GlobalTransformComponent& transform)
{
	auto translation_mat = Math::Translate(Math::Matrix4x4(), transform.Translation);
	auto scale_mat = Math::Scale(Math::Matrix4x4(), transform.Scale);

	auto rotation_mat = Math::ToMatrix4x4(transform.RotationQuat);

	return translation_mat * rotation_mat * scale_mat;
}

void TransformService::MergeTransforms(LocalTransformComponent& child, const LocalTransformComponent& parent)	//Видимость также меняется. Видимость ребёнка зависит от видимости родителя
{
	child.RotationQuat = Math::Normalize(child.RotationQuat * parent.RotationQuat); // Умножение кватернионов! Старое на новое
	child.Translation = parent.Translation + (parent.RotationQuat * child.Translation) * parent.Scale;
	child.Scale = child.Scale * parent.Scale;
	child.Visible = parent.Visible && child.Visible; // Логическое И вместо умножения
}

void TransformService::MarkDirty(ECS::EntityID id) {
    ECS::Registry& registry = m_eSpace->GetRegistry();
    ECS::HierarchyTree& tree = m_eSpace->GetTree();

    std::queue<ECS::EntityID> dirty_queue;
    dirty_queue.push(id); // Начинаем с корневого элемента

    while (!dirty_queue.empty()) {
        auto current = dirty_queue.front();
        dirty_queue.pop();

        // Помечаем текущий элемент
        if (registry.Has<LocalTransformComponent>(current)) {
            registry.Get<LocalTransformComponent>(current).is_dirty = true;
        }

        // Добавляем детей в очередь
        for (auto child : tree.GetChildren(current)) {
            dirty_queue.push(child);
        }
    }
}



void TransformService::Update(ECS::EntitySpace* eSpace)
{
	m_eSpace = eSpace;
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> TransformService::GetComponentFields(void* component)
{
	std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
	if (component == nullptr) return result;
	LocalTransformComponent* transform = static_cast<LocalTransformComponent*>(component);

	result["Translation"] = std::vector<float>({ transform->Translation.x, transform->Translation.y, transform->Translation.z });
	result["Scale"] = std::vector<float>({ transform->Scale.x, transform->Scale.y, transform->Scale.z });
	result["Rotation"] = std::vector<float>({ transform->RotationQuat.x, transform->RotationQuat.y, transform->RotationQuat.z, transform->RotationQuat.w });
	result["Visible"] = transform->Visible;
	return result;
}

void TransformService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
	LocalTransformComponent transform;

	for (auto [fieldName, fieldValue] : fields)
	{
		if (fieldName == "Visible") {
			if (auto wu = ECS::ESDL::GetAs<bool>(fieldValue)) {
				transform.Visible = *wu;
			}
		}
		else if (fieldName == "Translation") 
		{
			if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 3)transform.Translation = Math::Vector3(wu->at(0), wu->at(1), wu->at(2));
			}
		}
		else if (fieldName == "Scale")
		{
			if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 3)transform.Scale = Math::Vector3(wu->at(0), wu->at(1), wu->at(2));
			}
		}
		else if (fieldName == "Rotation")
		{
			if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 4) transform.RotationQuat = Math::Quaternion(wu->at(0), wu->at(1), wu->at(2), wu->at(3));
			}
		}
	}
	if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<LocalTransformComponent>(id, transform);
	else
	{
		ServiceLocator::Get<Logger>()->Log("(TransformService): transform's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
	}
}
