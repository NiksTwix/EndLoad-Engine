#include <Components\Graphical\SpriteComponent.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>
#include <Core\ServiceLocator.hpp>

SpriteService::SpriteService()
{
    m_stringType = "Sprite";
    m_ComponentType = typeid(SpriteComponent);
}

void SpriteService::Init()
{
}

void SpriteService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}

void SpriteService::Shutdown()
{
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> SpriteService::GetComponentFields(void* component)
{
	std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
	if (component == nullptr) return result;
	SpriteComponent* spr = static_cast<SpriteComponent*>(component);

	auto path = spr->TexturePath;
	auto color = spr->Color;
	result["Path"] = path;
	result["Color"] = std::vector<float>({ color.x,color.y,color.z,color.w });

	return result;
}

void SpriteService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
	SpriteComponent sprite;

	for (auto [fieldName, fieldValue] : fields)
	{
		if (fieldName == "Path") {
			if (auto wu = ECS::ESDL::GetAs<std::string>(fieldValue)) {
				sprite.TexturePath = *wu;
			}
			else {
				ServiceLocator::Get<Logger>()->Log("(SpriteService): sprite's deserialization error: value's type is not string. Entity " + std::to_string(id) + ".", Logger::Level::Error);
			}
		}
		if (fieldName == "Color") {
			if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(fieldValue)) {
				if (wu->size() >= 4) sprite.Color = Math::Vector4(wu->at(0), wu->at(1), wu->at(2), wu->at(3));
			}
		}
	}
	if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<SpriteComponent>(id, sprite);
	else
	{
		ServiceLocator::Get<Logger>()->Log("(SpriteService): sprite's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
	}
}
