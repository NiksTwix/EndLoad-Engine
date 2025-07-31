#include <Systems\Render/Render.hpp>
#include <Systems\Window\WindowManager.hpp>
#include <Services\Scene\SceneManager.hpp>
#include <Services\Debug\FPSCounter.hpp>
#include <Components\Basic\CameraComponent.hpp>
#include <Components\Basic\ScriptComponent.hpp>
#include <Systems\Modules\Modules.hpp>

Render::Render()
{
    Init();
};

Render::~Render()
{
    Shutdown();
}

void Render::Init()
{
    if (m_isValid) return;
    ServiceLocator::Set<FPSCounter, FPSCounter>();
    m_isValid = true;
}

void Render::Shutdown()
{
    if (!m_isValid) return;

    m_context->Shutdown();

    if (ServiceLocator::Get<Logger>() != nullptr) ServiceLocator::Get<Logger>()->Log(
        "(RenderSystem): RenderSystem shutdown completed. Render context shutdown.",
        Logger::Level::Warning
    );

    for (auto* module : _modules) 
    {
        delete module;
    }
    _modules.clear();
    m_isValid = false;
}

void Render::Update()
{
    if (!m_isValid || m_context == nullptr) return;

    auto wm = ServiceLocator::Get<WindowManager>();
    auto logger = ServiceLocator::Get<Logger>();

    for (auto& [windowId, sceneId] : m_scenes) {
        try {
            // Получаем окно
            std::shared_ptr<Window> window = wm->GetWindow(windowId);
            if (!window) {
                throw std::runtime_error("Window not found");
            }
            wm->SetRenderWindow(window.get());

            auto* scene = ServiceLocator::Get<SceneManager>()->GetContext(sceneId);

            for (auto viewport : scene->GetViewportService().GetViewports()) {
                if (viewport) RenderScene(viewport, scene);
            }
            window->SwapBuffers();
        }
        catch (const std::exception& e) {
            logger->Log(
                "(RenderSystem): render error (Window " + std::to_string(windowId) +
                "): " + e.what(),
                Logger::Level::Error
            );
        }
    }
    ServiceLocator::Get<FPSCounter>()->Update();
    //std::cout << ELServiceLocator::Get<FPSCounter>()->GetFPS() << "\n";
}

void Render::SetContext(std::unique_ptr<IRenderContext> new_context)
{
    m_context = std::move(new_context);
}

void Render::AttachSceneToWindow(WindowID window, SceneID scene)
{
    auto* wm = ServiceLocator::Get<WindowManager>();
    auto* sm = ServiceLocator::Get<SceneManager>();
    auto* l = ServiceLocator::Get<Logger>();

    //Проверка сервисов
    if (!wm || !sm || !l)
    {
        // Log only if logger is available
        if (l)
        {
            l->Log("(RenderSystem): critical service missing during scene attachment to window.",
                Logger::Level::Error);
        }
        return;
    }

    //Проверка окна
    if (!wm->IsWindowValid(window))
    {
        l->Log("(RenderSystem): failed to attach scene to window: Window " + std::to_string(window) + " is invalid.",
            Logger::Level::Error);
        return;
    }

    //Проверка сцены
    if (!sm->IsSceneValid(scene))
    {
        l->Log("(RenderSystem): failed to attach scene to window: Scene " + std::to_string(scene) + " is invalid.",
            Logger::Level::Error);
        return;
    }

    // Если окно уже работает с какой-то сценой
    if (m_scenes.count(window) > 0)
    {
        l->Log("(RenderSystem): scene with id " + std::to_string(m_scenes[window]) + " will be replaced by scene " + std::to_string(scene) + ".",
            Logger::Level::Warning);
    }

    // Прикрепляем сцену
    m_scenes[window] = scene;
    l->Log("(RenderSystem): scene with id " + std::to_string(scene) + " has been successfully attached to window " + std::to_string(window) + ".",
        Logger::Level::Success);
}

void Render::FirstFrameInitialization()
{
    if (!firstFrame) return;

    AddModule(new MeshRenderModule());
    AddModule(new SpriteRenderModule());
    AddModule(new UIRenderModule());
    firstFrame = false;

    oglDebugDraw = new OGLDebugDraw();
}

void Render::RenderScene(Viewport* viewport, SceneContext* scene)
{
    if (firstFrame) FirstFrameInitialization();
    float delta_time = ServiceLocator::Get<FPSCounter>()->GetFrameTime();      //Получение контекста
    scene->GetEntitySpace().UpdateAllServices();
    if (!scene || !viewport) return;//Обновление состояний сервисов компонентов
    auto& CSL = scene->GetEntitySpace().GetServiceLocator();
    
    CSL.Register<LocalTransformComponent, TransformService>()->UpdateGlobalTransforms();
    auto* m_cameraService = CSL.Register<CameraComponent,CameraService>();
    m_context->SetViewport(viewport);  //Установка вайпорта



    auto* cam = viewport->GetCamera().Get();
    if (cam) {
        m_cameraService->UpdateProjectionMatrix(*cam, viewport->GetResolution().x, viewport->GetResolution().y, cam->Projection_Type, cam->FOV);
    }
     
    m_cameraService->SetRenderCamera(viewport->GetCamera().GetEntityID());        //Настройка и обновление камер          
    m_cameraService->UpdateAllCameras(*scene);

    if (cam) {
        oglDebugDraw->SetCameraData(cam->Projection, cam->View, scene->GetEntitySpace().GetComponent<GlobalTransformComponent>(viewport->GetCamera().GetEntityID()).Translation);
        oglDebugDraw->DrawGrid(5, 20, Math::Vector4(1.0f, 0.5f, 0.5f, 1.0f));
    }

    for (auto* module : _modules)                                         //Отдаём работу модулям
    {
        module->Update(scene,delta_time);
    }
    
    CSL.Register<ScriptComponent,ScriptService>()->InvokeProcess(*scene, delta_time);         //Вызываем Process у скриптов
}
