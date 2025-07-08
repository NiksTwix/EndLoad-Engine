#pragma once
#include <Services\Resources\Graphics\TextureResource.hpp>
#include <Math\MathFunctions.hpp>
#include <ECS\ECS.hpp>

struct Sprite
{
	std::string TexturePath;
	Math::Vector4 Color = Math::Vector4(1.0f);
};

class SpriteService : public ECS::IComponentService
{
public:
	SpriteService();
	void Init() override;
	void Update(ECS::EntitySpace* eSpace) override;
	void Shutdown() override;
	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields) override;
};