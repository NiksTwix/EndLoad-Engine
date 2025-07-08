#pragma once
#include <Systems\Modules\IModule.hpp>
#include <Components\Sprite.hpp>
#include <Components\Transform.hpp>
#include <Components\Camera.hpp>

#include <ECS\ECS.hpp>
#include <Services\Resources\Graphics\Shader.hpp>
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
	std::unique_ptr<Shader> shader;
	
	float pixelPerUnit = 500;

	unsigned int plane_vao;
	int indices_count;
	void RenderSprite(ECS::EntityID id, Transform& transform, Sprite& sprite, SceneContext* context);
};




