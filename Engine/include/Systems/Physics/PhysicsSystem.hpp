#pragma once
#include <Core\IServices.hpp>
#include <Services/Scenes/SceneManager.hpp>

namespace Physics
{
	class PhysicsSystem : public Core::ISystem 
	{
	public:
		PhysicsSystem();
		void Init() override;
		void Shutdown() override;
		Core::SystemPriority GetPriority() override { return Core::SystemPriority::PHYSICS; }
		void Update() override;
	private:
		void UpdateScenePhysics(Scenes::SceneContext* context);
		void UpdateHiddenScenePhysics(Scenes::SceneContext* context);

		std::mutex m_physicsMutex;
	};
}