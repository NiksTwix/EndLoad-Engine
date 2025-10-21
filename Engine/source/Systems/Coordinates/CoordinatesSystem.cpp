#include <Systems/Coordinates/CoordinatesSystem.hpp>
#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Core\ServiceLocator.hpp>
#include <Components/Basic/TransformComponent.hpp>


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
		//TransformUpdate

		//World aabb Update
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