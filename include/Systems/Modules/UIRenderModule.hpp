#pragma once
#include <Systems\Modules\IModule.hpp>
#include <Components\Basic\TransformComponent.hpp>
#include <ECS\ECS.hpp>
#include <Components/UI/UIElementComponent.hpp>
#include <Components/UI/TextRenderComponent.hpp>
#include <Resources\Shader.hpp>
#include <memory>




class UIRenderModule : public IModule 
{
public:
	// Унаследовано через IModule

	UIRenderModule();

	uint8_t GetPriority() const override;
	void Update(SceneContext* context, float deltaTime) override;
private:
	unsigned int plane_vao;
	unsigned int text_plane_vao;
	int indices_count;

	ECS::ComponentServiceLocator* csl_;
	ECS::Registry* reg;
	std::shared_ptr<Shader> bgShader;

	std::shared_ptr<Shader> textShader;

	void DrawBackground(ECS::EntityID entity, UIElementComponent& uiElement);

	void DrawText(ECS::EntityID entity, UIElementComponent& uiElement);

};