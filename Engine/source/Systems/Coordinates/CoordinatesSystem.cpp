#include <Systems/Coordinates/CoordinatesSystem.hpp>
#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Core\ServiceLocator.hpp>
#include <Components/Basic/TransformComponent.hpp>
#include <Components/Graphics/MeshComponent.hpp>

namespace Coordinates 
{
	CoordinatesSystem::CoordinatesSystem()
	{
		Init();
	}
	void CoordinatesSystem::Init()
	{
		m_isValid = true;
	}
	void CoordinatesSystem::Shutdown()
	{
		m_isValid = false;
	}
	void CoordinatesSystem::Update()
	{
		std::lock_guard<std::mutex> lock(m_coordinatesMutex);
		auto k = Core::ServiceLocator::Get<Rendering::RenderSystem>()->GetAttachedScenes();
		auto sm = Core::ServiceLocator::Get<Scenes::SceneManager>();
		for (auto& scene : k)
		{
			auto* context = sm->GetContext(scene);
			if (context) UpdateScene(context);
		}
		m_updated = true;
		NotifyAll();
	}

	void CoordinatesSystem::UpdateScene(Scenes::SceneContext* context)
	{
		ECS::EntitySpace& e_space = context->GetEntitySpace();

		e_space.UpdateAllServices();	

		ECS::ComponentServiceLocator& csl = e_space.GetServiceLocator();

		auto* trs = csl.GetByService<Components::TransformComponentService>();

		std::vector<ECS::EntityID> dirty_aabbs;
		auto* ms = csl.GetByService<Components::MeshComponentService>();

		ECS::View<Components::LocalTransformComponent, Components::MeshComponent> view(e_space.GetRegistry());

		if (ms)
		{
			view.Each([&](ECS::EntityID entity_id, Components::LocalTransformComponent& lt, Components::MeshComponent& mesh)
				{
					if (Math::LengthSquared(mesh.aabb.Size) > 0 && lt.is_dirty)
					{
						dirty_aabbs.push_back(entity_id);
					}
				});
		}

		if (trs) trs->UpdateGlobalTransforms();
		
		for (auto& entity : dirty_aabbs) 
		{
			Components::GlobalTransformComponent& gt = e_space.GetComponent<Components::GlobalTransformComponent>(entity);
			ms->UpdateWorldAABB(e_space.GetComponent<Components::MeshComponent>(entity), gt.modelMatrix);
		}
	}

	void CoordinatesSystem::WaitForCompletion()
	{
		std::unique_lock lock(m_coordinatesMutex);
		m_update_cv.wait(lock, [this] { return m_updated.load(); });
		m_updated = false;
	}
	void CoordinatesSystem::NotifyAll()
	{
		m_update_cv.notify_all();	//Thread wake up, check condition { return m_updated; }, if condition is false again goes to sleep
	}



}