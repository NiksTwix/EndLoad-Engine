#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Services\Scenes\SceneManager.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Services\Diagnostics\ProcessObserver\ProcessObserver.hpp>
#include <Services/ResourcesManager/ResourcesManager.hpp>
#include <Systems/Modules/Rendering/RenderModule3D.hpp>
namespace Rendering
{
	RenderSystem::RenderSystem()
	{
		Init();
	}
	RenderSystem::~RenderSystem()
	{
		Shutdown();
	}
	void RenderSystem::Init()
	{
		if (m_isValid) return;

		m_isValid = true;
		m_firstFrame = true;
	}
	void RenderSystem::Shutdown()
	{
		if (!m_isValid) return;

		m_modules.clear();

		m_isValid = false;
	}
	void RenderSystem::Update()
	{
		if (!m_isValid) return;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto& logger = Diagnostics::Logger::Get();
		for (auto& [windowId, sceneId] : m_scenes) {
			try {
				if (m_scenes_changed)
				{
					m_scenes_changed = false;
					break;
				}
				auto* window = wm->GetWindow(windowId);
				if (!window) {
					DetachSceneFromWindow(windowId);
					continue;
				}
				if (window->GetWindowState() == Windows::WindowState::Collapsed) continue;
				//m_context = window->GetContext();
				wm->SetRenderWindow(window->GetID());
				//               
				auto* scene = Core::ServiceLocator::Get<Scenes::SceneManager>()->GetContext(sceneId);
				
				for (auto viewport : scene->GetViewportService().GetViewports()) {
					if (viewport)
					{
						auto rw = wm->GetRenderWindow();
						rw->GetGraphicsDevice()->SetRenderViewport(viewport);
						RenderScene(viewport, scene);
					}
				}
				window->SwapFrameBuffers();
				Core::ServiceLocator::Get<Resources::ResourcesManager>()->ClearWindowCache(window->GetID());
			}
			catch (const std::exception& e) {
				Diagnostics::Logger::Get().SendMessage("(RenderSystem) Rendering error (Window " + std::to_string(windowId) + "): " + e.what() + ".", Diagnostics::MessageType::Error);
			}
		}
		
	}
	void RenderSystem::AttachSceneToWindow(Windows::WindowID window, Scenes::SceneID scene)
	{
		auto* wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto* sm = Core::ServiceLocator::Get<Scenes::SceneManager>();
		auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
		auto& l = Diagnostics::Logger::Get();

		if (!wm || !sm)
		{
			l.SendMessage("(RenderSystem) Critical service missing during scene attachment to window.", Diagnostics::MessageType::Error);
			return;
		}
		if (!wm->IsExists(window))
		{
			l.SendMessage("(RenderSystem) Failed to attach scene to window: Window " + std::to_string(window) + " is invalid.", Diagnostics::MessageType::Error);
			return;
		}
		if (!sm->IsSceneValid(scene))
		{
			l.SendMessage("(RenderSystem) Failed to attach scene to window: Scene " + std::to_string(scene) + " is invalid.", Diagnostics::MessageType::Error);
			return;
		}

		auto* new_scene = sm->GetContext(scene);

		if (new_scene->GetState() == Scenes::SceneState::Attached) 
		{
			l.SendMessage("(RenderSystem) Scene with id " + std::to_string(m_scenes[window]) + " has already attached.", Diagnostics::MessageType::Error);
			return;
		}

		if (m_scenes.count(window) > 0)
		{
			l.SendMessage("(RenderSystem) Scene with id " + std::to_string(m_scenes[window]) + " will be replaced by scene " + std::to_string(scene) + ".", Diagnostics::MessageType::Warning);
			DetachSceneFromWindow(window);
		}
		m_scenes[window] = scene;

		auto r_window = wm->GetRenderWindow();
		
		wm->SetRenderWindow(window);

		
		rm->AttachStaticResources(new_scene->GetStaticResources());
		new_scene->SetState(Scenes::SceneState::Attached);
		//if (r_window != nullptr) wm->SetRenderWindow(r_window->GetID());
		l.SendMessage("(RenderSystem) Scene with id " + std::to_string(scene) + " has been successfully attached to window " + std::to_string(window) + ".", Diagnostics::MessageType::Info);
		m_scenes_changed = true;
	}
	void RenderSystem::DetachSceneFromWindow(Windows::WindowID window)
	{
		if (m_scenes.count(window))
		{
			m_scenes_changed = true;
			auto* current_scene = Core::ServiceLocator::Get<Scenes::SceneManager>()->GetContext(m_scenes[window]);

			if (current_scene) current_scene->SetState(Scenes::SceneState::NotAttached);

			m_scenes.erase(window);

			auto* wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
			if (wm->IsExists(window)) 
			{
				auto r_window = wm->GetRenderWindow();
				wm->SetRenderWindow(window);
				auto* rm = Core::ServiceLocator::Get<Resources::ResourcesManager>();
				rm->GetActiveFrame()->ClearStaticResources();
				//if (r_window != nullptr) wm->SetRenderWindow(r_window->GetID());
			}
		}
	}
	Scenes::SceneID RenderSystem::GetAttachedScene(Windows::WindowID window)
	{
		if (m_scenes.count(window)) return m_scenes[window];
		return Definitions::InvalidID;
	}
	std::vector<Scenes::SceneID> RenderSystem::GetAttachedScenes() const
	{
		std::vector<Scenes::SceneID> result;
		for (auto& [_, scene] : m_scenes) 
		{
			result.push_back(scene);
		}
		return result;
	}
	void RenderSystem::FirstFrameInitialization()
	{
		if (!m_firstFrame) return;
		auto rm3 = std::make_shared<RenderModule3D>();
		rm3->SetPriority(0);
		
		//auto srm = std::make_shared<SpriteRenderModule>();
		//srm->SetPriority(1);
		//
		//auto uirm = std::make_shared<UIRenderModule>();
		//uirm->SetPriority(0);

		AddModule(rm3);
		//AddModule(srm);
		//AddModule(uirm);
		m_firstFrame = false;
	}
	void RenderSystem::RenderScene(Viewports::Viewport* viewport, Scenes::SceneContext* scene)
	{
		if (m_firstFrame) FirstFrameInitialization();
		if (scene->GetEntitySpace().GetTree().GetNodesCount() == 0) return;
		float delta_time = Diagnostics::ProcessObserver::GetFrameTimeMSST();      
		if (!scene || !viewport) return;
		auto& CSL = scene->GetEntitySpace().GetServiceLocator();
		auto* m_cameraService = CSL.GetByService<Components::CameraComponentService>();
		auto cam = viewport->GetCamera().GetEntityID();
		if (cam != Definitions::InvalidID) {
			m_cameraService->UpdateCameraData(scene, cam);
		}
		UpdateModules();

		//CSL.Register<ScriptComponent, ScriptService>()->InvokeProcess(*scene, delta_time);        
		//CSL.Register<ELScriptComponent, ELScriptService>()->InvokeProcess(*scene, delta_time);
	}
}