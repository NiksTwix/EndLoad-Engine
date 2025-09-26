#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Services\Scenes\SceneManager.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Services\Diagnostics\ProcessObserver\ProcessObserver.hpp>

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
					if (viewport) RenderScene(viewport, scene);
				}
				window->SwapFrameBuffers();
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
		if (m_scenes.count(window) > 0)
		{
			l.SendMessage("(RenderSystem) Scene with id " + std::to_string(m_scenes[window]) + " will be replaced by scene " + std::to_string(scene) + ".", Diagnostics::MessageType::Warning);
		}
		m_scenes[window] = scene;

		l.SendMessage("(RenderSystem) Scene with id " + std::to_string(scene) + " has been successfully attached to window " + std::to_string(window) + ".", Diagnostics::MessageType::Info);
		m_scenes_changed = true;
	}
	void RenderSystem::DetachSceneFromWindow(Windows::WindowID window)
	{
		if (m_scenes.count(window))
		{
			m_scenes_changed = true;
			m_scenes.erase(window);
		}
	}
	Scenes::SceneID RenderSystem::GetAttachedScene(Windows::WindowID window)
	{
		if (m_scenes.count(window)) return m_scenes[window];
		return Definitions::InvalidID;
	}
	void RenderSystem::FirstFrameInitialization()
	{
		if (!m_firstFrame) return;

		//AddModule(std::make_shared<MeshRenderModule>());
		//AddModule(std::make_shared<SpriteRenderModule>());
		//AddModule(std::make_shared<UIRenderModule>());
		m_firstFrame = false;
	}
	void RenderSystem::RenderScene(Viewports::Viewport* viewport, Scenes::SceneContext* scene)
	{
		if (m_firstFrame) FirstFrameInitialization();
		float delta_time = Diagnostics::ProcessObserver::GetFrameTimeMSST();      
		scene->GetEntitySpace().UpdateAllServices();
		if (!scene || !viewport) return;
		auto& CSL = scene->GetEntitySpace().GetServiceLocator();

		//CSL.Register<LocalTransformComponent, TransformService>()->UpdateGlobalTransforms();
		//auto* m_cameraService = CSL.Register<CameraComponent, CameraService>();
		//m_context->SetViewport(viewport);  //Óñòàíîâêà âàéïîðòà



		auto* cam = viewport->GetCamera().Get();
		if (cam) {
			//m_cameraService->UpdateProjectionMatrix(*cam, viewport->GetResolution().x, viewport->GetResolution().y, cam->Projection_Type, cam->FOV);
		}

		//m_cameraService->SetRenderCamera(viewport->GetCamera().GetEntityID());                  
		//m_cameraService->UpdateAllCameras(*scene);
		UpdateModules();

		//CSL.Register<ScriptComponent, ScriptService>()->InvokeProcess(*scene, delta_time);        
		//CSL.Register<ELScriptComponent, ELScriptService>()->InvokeProcess(*scene, delta_time);
	}
}