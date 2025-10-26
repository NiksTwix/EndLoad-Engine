#include <Services\Scenes\SceneManager.hpp>
#include <Components\Components.hpp>

namespace Scenes 
{
	SceneManager::~SceneManager()
	{
	}

	SceneManager::SceneManager()
	{
		Init();
	}

	SceneContext* SceneManager::GetContext(SceneID id) const
	{
		if (scenes.count(id) == 0) return nullptr;

		return scenes.at(id).get();
	}

	SceneContext* SceneManager::CreateContext(std::string name)
	{
		auto id = last_id++;
		scenes.try_emplace(id, std::make_unique<SceneContext>(name, id));
		RegisterBasicCS(scenes[id].get());
		return scenes[id].get();
	}

	void SceneManager::DeleteContext(SceneID id)
	{
		if (scenes.count(id) == 0) return;
		scenes.erase(id);
	}

	bool SceneManager::IsSceneValid(SceneID id) const
	{
		return scenes.count(id) != 0;
	}

	void SceneManager::Shutdown()
	{
		scenes.clear();
	}

	void SceneManager::RegisterBasicCS(SceneContext* scene)		//CS - ComponentServices
	{
		//Registration

		ECS::CSL& csl = scene->GetEntitySpace().GetServiceLocator();

		csl.Register<Components::LocalTransformComponent, Components::TransformComponentService>();
		csl.Register<Components::CameraComponent, Components::CameraComponentService>();
		csl.Register<Components::MeshComponent, Components::MeshComponentService>();
		csl.Register<Components::MaterialComponent, Components::MaterialComponentService>();
		//csl.Register<SpriteComponent, SpriteService>();
		//csl.Register<ScriptComponent, ScriptService>();
		csl.Register<Components::ELScriptComponent, Components::ELScriptComponentService>();
		//csl.Register<UIElementComponent, UIElementService>();
		//csl.Register<TextRenderComponent, TextRenderService>();
		//csl.Register<EventContainerComponent, EventContainerService>();

		scene->GetEntitySpace().UpdateAllServices();
	}
}

