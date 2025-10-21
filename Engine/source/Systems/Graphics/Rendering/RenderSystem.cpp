#include <Systems\Graphics\Rendering\RenderSystem.hpp>
#include <Services\Scenes\SceneManager.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Services\Diagnostics\ProcessObserver\ProcessObserver.hpp>
#include <Services/ResourcesManager/ResourceManager.hpp>
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
					if (viewport) RenderScene(viewport, scene);
				}
				window->SwapFrameBuffers();
				Core::ServiceLocator::Get<Resources::ResourceManager>()->ClearWindowCache(window->GetID());
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
		auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
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

		auto r_window = wm->GetRenderWindow();
		
		wm->SetRenderWindow(window);
		
		rm->AttachStaticResources(sm->GetContext(scene)->GetStaticResources());
		
		if (r_window != nullptr) wm->SetRenderWindow(r_window->GetID());
		l.SendMessage("(RenderSystem) Scene with id " + std::to_string(scene) + " has been successfully attached to window " + std::to_string(window) + ".", Diagnostics::MessageType::Info);
		m_scenes_changed = true;
	}
	void RenderSystem::DetachSceneFromWindow(Windows::WindowID window)
	{
		if (m_scenes.count(window))
		{
			m_scenes_changed = true;
			m_scenes.erase(window);

			auto* wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
			if (wm->IsExists(window)) 
			{
				auto r_window = wm->GetRenderWindow();
				wm->SetRenderWindow(window);
				auto* rm = Core::ServiceLocator::Get<Resources::ResourceManager>();
				rm->GetActiveFrame()->ClearStaticResources();
				if (r_window != nullptr) wm->SetRenderWindow(r_window->GetID());
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
		//auto mrm = std::make_shared<MeshRenderModule>();
		//mrm->SetPriority(0);
		//
		//auto srm = std::make_shared<SpriteRenderModule>();
		//srm->SetPriority(1);
		//
		//auto uirm = std::make_shared<UIRenderModule>();
		//uirm->SetPriority(0);

		//AddModule(mrm);
		//AddModule(srm);
		//AddModule(uirm);
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
		auto* m_cameraService = CSL.GetByService<Components::CameraComponentService>();
		//m_context->SetViewport(viewport);  //Óñòàíîâêà âàéïîðòà


		auto cam = viewport->GetCamera().GetEntityID();
		if (cam != Definitions::InvalidID) {
			//m_cameraService->UpdateProjectionMatrix(*cam, viewport->GetResolution().x, viewport->GetResolution().y, cam->Projection_Type, cam->FOV);
			m_cameraService->UpdateCameraData(scene, cam);	//WARNING РАССИНХРОН С ТРАНСФОРМАМИ (ОНИ ОБНОВЛЯЮТСЯ В ФИЗИКЕ!), отставание на 1 кадр, можно перенести в отдельную систему Transform+Camera ну, понятно думаю
		}

		//m_cameraService->SetRenderCamera(viewport->GetCamera().GetEntityID());                  
		//m_cameraService->UpdateAllCameras(*scene);
		UpdateModules();

		//CSL.Register<ScriptComponent, ScriptService>()->InvokeProcess(*scene, delta_time);        
		//CSL.Register<ELScriptComponent, ELScriptService>()->InvokeProcess(*scene, delta_time);
	}
}