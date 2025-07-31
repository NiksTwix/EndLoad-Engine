#pragma once
#include <Systems\Modules\IModule.hpp>
#include <ECS\ECS.hpp>
#include <Components\Graphical\MaterialComponent.hpp>
#include <Components\Graphical\MeshComponent.hpp>
#include <Components\Basic\CameraComponent.hpp>
#include <Components\Basic\TransformComponent.hpp>

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

	void RenderMesh(ECS::EntityID id, LocalTransformComponent& transform, MeshComponent& mesh, MaterialComponent& material, SceneContext* context);
};