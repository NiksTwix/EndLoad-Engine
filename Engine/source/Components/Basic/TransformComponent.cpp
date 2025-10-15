#include <Components/Basic/TransformComponent.hpp>
#include <queue>
#include <Services/Scenes/SceneContext.hpp>


namespace Components 
{
	TransformComponentService::TransformComponentService()
	{
		m_stringType = "Transform";
		m_ComponentType = typeid(LocalTransformComponent);
	}

	void TransformComponentService::Init()
	{

	}

	void TransformComponentService::Shutdown()
	{
	}

	void TransformComponentService::UpdateSingleGlobalTransform(ECS::EntityID id, Scenes::SceneContext& context)
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

		gt.visible = local.visible;
		gt.origin = local.origin;
		gt.basis.SetRotation(local.basis.GetRotation());
		gt.basis.SetScale(local.basis.GetScale());
		gt.modelMatrix = GetModelMatrix(gt);

		registry.Add<GlobalTransformComponent>(id, gt);
	}

	void TransformComponentService::UpdateGlobalTransforms()
	{
		if (m_eSpace == nullptr) return;

		ECS::Registry& registry = m_eSpace->GetRegistry();
		ECS::HierarchyTree& tree = m_eSpace->GetTree();
		ECS::ComponentServiceLocator& csl = m_eSpace->GetServiceLocator();

		std::queue<ECS::EntityID> processQueue;
		for (auto child : tree.GetRootChildren())
		{
			auto* t = registry.TryGet<LocalTransformComponent>(child);
			if (t != nullptr && t->is_dirty)
			{
				GlobalTransformComponent gt;
				gt.basis.SetRotation(t->basis.GetRotation());
				gt.origin = t->origin;
				gt.basis.SetScale(t->basis.GetScale());
				gt.visible = t->visible;
				gt.modelMatrix = GetModelMatrix(gt);
				registry.Add<GlobalTransformComponent>(child, gt);
				processQueue.push(child);
				t->is_dirty = false;
			}
			else if (t != nullptr && !t->is_dirty)
			{
				processQueue.push(child);
			}
		}
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
					gt.basis.SetRotation(Math::Normalize(t->basis.GetRotation() * parent->basis.GetRotation()));
					gt.origin = parent->origin + (parent->basis.GetRotation() * t->origin) * parent->basis.GetScale();
					gt.basis.SetScale(t->basis.GetScale() * parent->basis.GetScale());
					gt.visible = parent->visible && t->visible;
					gt.modelMatrix = GetModelMatrix(gt);
					registry.Add<GlobalTransformComponent>(child, gt);
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


	void TransformComponentService::Translate(ECS::EntityID entity, Math::Vector3 translation)
	{
		auto& translated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
		translated.origin += translation;
		//TODO isDirty
		MarkDirty(entity);
	}

	void TransformComponentService::Scale(ECS::EntityID entity, Math::Vector3 scale)
	{
		auto& scaled = m_eSpace->GetComponent<LocalTransformComponent>(entity);
		scaled.basis.Scale(scale);
		MarkDirty(entity);
	}

	void TransformComponentService::RotateLocal(ECS::EntityID entity, Math::Vector3 axis, float angleRadians) {
		if (Math::LengthSquared(axis) < FLT_EPSILON) return;
		auto& rotated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
		rotated.basis.RotateLocal(axis, angleRadians);
		MarkDirty(entity);
	}

	void TransformComponentService::RotateGlobal(ECS::EntityID entity, Math::Vector3 axis, float angleRadians) {
		if (Math::LengthSquared(axis) < FLT_EPSILON) return;
		auto& rotated = m_eSpace->GetComponent<LocalTransformComponent>(entity);
		rotated.basis.RotateGlobal(axis, angleRadians);
		MarkDirty(entity);
	}

	Math::Matrix4x4 TransformComponentService::GetModelMatrix(LocalTransformComponent& transform)
	{
		auto translation_mat = Math::Translate(Math::Matrix4x4(), transform.origin);
		auto scale_mat = Math::Scale(Math::Matrix4x4(), transform.basis.GetScale());

		auto rotation_mat = Math::ToMatrix4x4(transform.basis.GetRotation());

		return translation_mat * rotation_mat * scale_mat;
	}

	Math::Matrix4x4 TransformComponentService::GetModelMatrix(GlobalTransformComponent& transform)
	{
		auto translation_mat = Math::Translate(Math::Matrix4x4(), transform.origin);
		auto scale_mat = Math::Scale(Math::Matrix4x4(), transform.basis.GetScale());

		auto rotation_mat = Math::ToMatrix4x4(transform.basis.GetRotation());

		return translation_mat * rotation_mat * scale_mat;
	}

	void TransformComponentService::MergeTransforms(LocalTransformComponent& child, const LocalTransformComponent& parent)	
	{
		child.basis.SetRotation(Math::Normalize(child.basis.GetRotation() * parent.basis.GetRotation()));
		child.origin = parent.origin + (parent.basis.GetRotation() * child.origin) * parent.basis.GetScale();
		child.basis.Scale(parent.basis.GetScale());
		child.visible = parent.visible && child.visible; 
	}

	void TransformComponentService::MarkDirty(ECS::EntityID id) {
		ECS::Registry& registry = m_eSpace->GetRegistry();
		ECS::HierarchyTree& tree = m_eSpace->GetTree();

		std::queue<ECS::EntityID> dirty_queue;
		dirty_queue.push(id); 

		while (!dirty_queue.empty()) {
			auto current = dirty_queue.front();
			dirty_queue.pop();

			if (registry.Has<LocalTransformComponent>(current)) {
				registry.Get<LocalTransformComponent>(current).is_dirty = true;
			}

			for (auto child : tree.GetChildren(current)) {
				dirty_queue.push(child);
			}
		}
	}

	void TransformComponentService::Update(ECS::EntitySpace* eSpace)
	{
		m_eSpace = eSpace;
	}
}