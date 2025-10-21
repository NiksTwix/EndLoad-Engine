#pragma once
#include <Systems\Physics\PhysicsSystem.hpp>
#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Systems/Coordinates/CoordinatesSystem.hpp>
#include <Core\ServiceLocator.hpp>
#include <Components/Basic/TransformComponent.hpp>

namespace Physics 
{
	PhysicsSystem::PhysicsSystem()
	{
		Init();
	}
	void PhysicsSystem::Init()
	{
		m_isValid = true;
	}
	void PhysicsSystem::Shutdown()
	{
		m_isValid = false;
	}
	void PhysicsSystem::Update()
	{
		auto* coords = Core::ServiceLocator::Get<Coordinates::CoordinatesSystem>();
		if (!coords) return;
		//Wait ending of coords update
		coords->WaitForCompletion();

		auto k = Core::ServiceLocator::Get<Rendering::RenderSystem>()->GetAttachedScenes();
		auto sm = Core::ServiceLocator::Get<Scenes::SceneManager>();
		for (auto& scene : k)
		{
			auto* context = sm->GetContext(scene);
			if (context) UpdateScenePhysics(context);
		}
	}
	void PhysicsSystem::UpdateScenePhysics(Scenes::SceneContext* context)
	{
	}
	void PhysicsSystem::UpdateHiddenScenePhysics(Scenes::SceneContext* context)
	{
	}
}