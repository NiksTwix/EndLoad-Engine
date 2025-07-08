#pragma once
#include <vector>
#include <unordered_map>
#include <mutex>
#include <Systems/Window/GLFWWindow.hpp>
#include <Systems/Render/Contexts/IRenderContext.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Systems\Render\Viewport.hpp>


class SceneManager;
class OGLDebugDraw;
class SettingsManager;

class ComponentServiceLocator;
class TransformService;
class MeshService;
class MaterialService;
class CameraService;
class Camera;



class OpenGLContext : public IRenderContext 
{

public:
	OpenGLContext();
	void Init() override;
	void Update(Viewport* viewport, SceneID scene) override;
    
    void Shutdown();


    void ClearBuffers() override;

    void SetViewport(Viewport* viewport)  override;
    unsigned int SetupMesh(std::vector<Math::Vertex> vertices, std::vector<unsigned int> indices) override;
    void DestroyMesh(unsigned int vao) override;
    void DrawElements(unsigned int vao, int index_count) override;

    void BindTexture(unsigned int tblock, unsigned int value) override;
    void Set(RCSettings set, RCSettingsValues value) override;

private: 
    void Render(SceneID scene_id);

    void Draw3DObjects(SceneContext* context);

    void Draw2DObjects(SceneContext* context);

    void DrawUIObjects(SceneContext* context, Camera& cam);

    SceneManager* manager = nullptr;

    //OGLDebugDraw* debugDraw = nullptr;

    std::mutex m_mutex;
    ComponentServiceLocator* CSL = nullptr;

    Viewport* m_currentViewport = nullptr;

    TransformService* m_transformService = nullptr;
    MeshService* m_meshService = nullptr;
    MaterialService* m_materialService = nullptr;
    CameraService* m_cameraService = nullptr;

    SettingsManager* m_settings_manager = nullptr;

    // Унаследовано через IRenderContext
    Viewport* GetViewport() override;


    std::unordered_map<GLuint, std::vector<GLuint>> ActiveGBuffers;

    // Унаследовано через IRenderContext
    void ResetBuffers() override;
};