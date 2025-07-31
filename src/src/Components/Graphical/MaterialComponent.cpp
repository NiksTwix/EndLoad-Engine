#include <Components\Graphical\MaterialComponent.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Services\Resources\ResourceManager.hpp>
#include <Resources\TextureResource.hpp>
#include <Resources\ShaderResource.hpp>
#include <sstream>
#include <Resources/GLShader.hpp>
#include <Systems\Render\Render.hpp>
#include <Services\Utility\Functions.hpp>

MaterialService::MaterialService()
{
    m_stringType = "Material";
    m_ComponentType = typeid(MaterialComponent);
}

void MaterialService::Init()
{
    
}

void MaterialService::Shutdown()
{
}

MaterialComponent MaterialService::CreateMaterial(const std::string& path_to_shader)
{
    auto shader = ServiceLocator::Get<ResourceManager>()->Load<ShaderResource>(path_to_shader);
    if (!shader) return MaterialComponent();

    MaterialComponent mat;
    mat.shader = shader->GetContent();

    return mat;
}

void MaterialService::Bind(MaterialComponent& material) const
{
    if (!material.shader || !material.shader->IsValid()) {
        ServiceLocator::Get<Logger>()->Log("(MaterialService): invalid shader in material", Logger::Level::Warning);
        return;
    }

    auto* render = ServiceLocator::Get<Render>();

    material.shader->Bind();

    auto* resmanager = ServiceLocator::Get<ResourceManager>();
    if (!resmanager) {
        ServiceLocator::Get<Logger>()->Log("(MaterialService): resourceManager not available", Logger::Level::Error);
        return;
    }

    int texture_index = 0; // Счётчик текстурных юнитов

    for (const auto& [name, value] : material.uniforms) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::string>) {
                // Оптимизация: кешируем результат Get<TextureResource>
                if (auto* texture = resmanager->Get<TextureResource>(arg)) {
                    if (texture->IsValid()) {
                        render->GetContext()->BindTexture(texture_index, texture->GetID());
                        material.shader->SetUniform(name, texture_index);
                        texture_index++;
                    }
                }
            }
            else {
                material.shader->SetUniform(name, arg);
            }
            }, value.data);
    }
}

void MaterialService::Unbind(MaterialComponent& material) const
{
    if (!material.shader || !material.shader->IsValid()) {
        ServiceLocator::Get<Logger>()->Log("(MaterialService): invalid shader in material", Logger::Level::Warning);
        return;
    }
    material.shader->Unbind();
}


void MaterialService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> MaterialService::GetComponentFields(void* component)
{
    std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
    if (component == nullptr) return result;

    MaterialComponent* material = static_cast<MaterialComponent*>(component);

    auto shader_path = material->shader->GetPath();         //TODO Доработать, чтобы сохраняло исходный текст
    result["Path"] = shader_path;
    for (auto& [name,value] : material->uniforms)
    {
        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, int>) {
                result[name] = value;
            }
            else if constexpr (std::is_same_v<T, float>) {
                result[name] = value;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                result[name] = value;
            }
            else if constexpr (std::is_same_v<T, Math::Vector2>) {
                result[name] = value.ToArray();
            }
            else if constexpr (std::is_same_v<T, Math::Vector3>) {
                result[name] = value.ToArray();
            }
            else if constexpr (std::is_same_v<T, Math::Vector4>) {
                result[name] = value.ToArray();
            }
            else if constexpr (std::is_same_v<T, Math::Matrix4x4>) {
                result[name] = value.ToArray();                  //WARNING
            }
            else if constexpr (std::is_same_v<T, std::string>) {            //path
                result[name] = value;
            }
            }, value.data);
    }

    return result;
}

void MaterialService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
    MaterialComponent mat;

    for (auto [fieldName, fieldValue] : fields)
    {
        if (fieldName == "Path") {
            if (auto wu = ECS::ESDL::GetAs<std::string>(fieldValue)) {
                mat = CreateMaterial(*wu);
            }
            else {
                ServiceLocator::Get<Logger>()->Log("(MaterialService): material's deserialization error: value's type is not string. Entity " + std::to_string(id) + ".", Logger::Level::Error);
            }
        }
        else //Это юниформы
        {
            if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(fieldValue)) {
                switch (wu->size())
                {
                case 2:
                    mat.uniforms[fieldName].data = Math::Vector2::FromArray(*wu);
                    break;
                case 3:
                    mat.uniforms[fieldName].data = Math::Vector3::FromArray(*wu);
                    break;
                case 4:
                    mat.uniforms[fieldName].data = Math::Vector4::FromArray(*wu);
                    break;
                case 16:
                    mat.uniforms[fieldName].data = Math::Matrix4x4::FromArray(*wu);
                    break;
                default:
                    break;
                }
            }
            else if (auto wu = ECS::ESDL::GetAs<float>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::GetAs<int>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::GetAs<bool>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::GetAs<std::string>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else {
                ServiceLocator::Get<Logger>()->Log("(MaterialService): material's deserialization error: undefined type. Entity " + std::to_string(id) + ".", Logger::Level::Error);
            }
        }
    }
    if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<MaterialComponent>(id, mat);
    else
    {
        ServiceLocator::Get<Logger>()->Log("(MaterialService): material's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
    }

}
