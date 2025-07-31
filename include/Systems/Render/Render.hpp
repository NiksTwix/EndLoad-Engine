#pragma once
#include <Core/ISystem.hpp>
#include <Core/ServiceLocator.hpp>
#include <Core/Logger.hpp>
#include <Systems/Render/Contexts/IRenderContext.hpp>
#include <memory>
#include <vector>

#include <Services\Debug\OGLDebugDraw.hpp> //WARNING Костыль

using SceneID = size_t;
using WindowID = int;
class Window;




class Render : public ISystem
{
public:
    // Разрешаем перемещение (если нужно)
    Render(Render&&) = default;
    Render& operator=(Render&&) = default;

    Render();
    ~Render();

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual SystemPriority GetPriority() override { return SystemPriority::RENDER; }
    void Update() override;

    void SetContext(std::unique_ptr<IRenderContext> new_context);
    IRenderContext* GetContext() { return m_context.get(); }
    void AttachSceneToWindow(WindowID window, SceneID scene);

private:
    std::unique_ptr<IRenderContext> m_context;
    std::unordered_map<WindowID, SceneID> m_scenes;

    bool firstFrame = true;

    void FirstFrameInitialization();
    void RenderScene(Viewport* viewport, SceneContext* scene);

    OGLDebugDraw* oglDebugDraw;//WARNING Костыль
};