#pragma once
#include <Systems\Modules\IModule.hpp>
#include <ECS\ECS.hpp>
#include <Components\Material.hpp>
#include <Components\Mesh.hpp>
#include <Components\Camera.hpp>
#include <Components\Transform.hpp>

class SettingsManager;
class ComponentServiceLocator;

class SceneContext;

class MeshRenderModule : public IModule 
{
	// Унаследовано через IModule
public:
	MeshRenderModule();
	uint8_t GetPriority() const override;
	void Update(SceneContext* context, float deltaTime) override;

private:
	TransformService* m_transformService = nullptr;
	MeshService* m_meshService = nullptr;
	MaterialService* m_materialService = nullptr;
	CameraService* m_cameraService = nullptr;

	SettingsManager* m_settingsManager = nullptr;

	void RenderMesh(ECS::EntityID id, Transform& transform, Mesh& mesh, Material& material, SceneContext* context);
};