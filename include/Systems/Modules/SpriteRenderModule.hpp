#pragma once
#include <Systems\Modules\IModule.hpp>
#include <Components\Graphical\SpriteComponent.hpp>
#include <Components\Basic\TransformComponent.hpp>
#include <Components\Basic\CameraComponent.hpp>
					 
#include <ECS\ECS.hpp>
#include <Resources\Shader.hpp>
#include <memory>

class SceneContext;


class SpriteRenderModule : public IModule 
{
public:
	SpriteRenderModule();
	uint8_t GetPriority() const override;
	void Update(SceneContext* context, float deltaTime) override;

	~SpriteRenderModule();

private:
	TransformService* m_transformService = nullptr;
	CameraService* m_cameraService = nullptr;
	std::shared_ptr<Shader> shader;
	
	float pixelPerUnit = 500;

	unsigned int plane_vao;
	int indices_count;
	void RenderSprite(ECS::EntityID id, LocalTransformComponent& transform, SpriteComponent& sprite, SceneContext* context);
};




