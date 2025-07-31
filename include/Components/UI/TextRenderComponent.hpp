#pragma once
#include <ECS\ECS.hpp>
#include <Resources\FontResource.hpp>
#include <memory>



enum class StringAlignment
{
	Near,
	Middle,
	Far
};
struct TextRenderComponent 
{
	std::shared_ptr<FontResource> Font;
	Math::Vector2 RenderScale = Math::Vector2(1); //Управляет размером шрифта в шейдере и при расчёте расстояния.
	StringAlignment XAlignment = StringAlignment::Near;
	StringAlignment YAlignment = StringAlignment::Near;

	int XOffset = 0;
	int YOffset = 0;
	std::string Text;

	Math::Vector4 Color = Math::Vector4(1);
};





class TextRenderService : public ECS::IComponentService
{
public:
	void Init() override;
	void Update(ECS::EntitySpace* eSpace) override;
	void Shutdown() override;
	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields) override;

	Math::Vector2 CalculateTextSize(const TextRenderComponent& text);
	Math::Vector2 GetAlignmentOffset(const TextRenderComponent& text, const Math::Vector2& uiSize, const Math::Vector2& textSize);
	float GetTextBaseOffset(const TextRenderComponent& text);
};