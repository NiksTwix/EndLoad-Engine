#include <Components\Material.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Services\Resources\ResourceManager.hpp>
#include <Services\Resources\Graphics\TextureResource.hpp>
#include <Services\Resources\Graphics\ShaderResource.hpp>
#include <sstream>
#include <Services/Resources/Graphics/GLShader.hpp>
#include <Systems\Render\Render.hpp>
#include <Services\Utility\Functions.hpp>

MaterialService::MaterialService()
{
    m_stringType = "Material";
    m_ComponentType = typeid(Material);
}

void MaterialService::Init()
{
    
}

void MaterialService::Shutdown()
{
}

Material MaterialService::CreateMaterial(const std::string& path_to_shader)
{
    auto shader = ServiceLocator::Get<ResourceManager>()->Load<ShaderResource>(path_to_shader);
    if (!shader) return Material();

    Material mat;
    mat.shader = shader->GetContent();

    return mat;
}

void MaterialService::Bind(Material& material) const
{
    if (!material.shader || !material.shader->IsValid()) {
        ServiceLocator::Get<Logger>()->Log("Invalid shader in material", Logger::Level::Warning);
        return;
    }

    auto* render = ServiceLocator::Get<Render>();

    material.shader->Bind();

    auto* resmanager = ServiceLocator::Get<ResourceManager>();
    if (!resmanager) {
        ServiceLocator::Get<Logger>()->Log("ResourceManager not available", Logger::Level::Error);
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

void MaterialService::Unbind(Material& material) const
{
    if (!material.shader || !material.shader->IsValid()) {
        ServiceLocator::Get<Logger>()->Log("Invalid shader in material", Logger::Level::Warning);
        return;
    }
    material.shader->Unbind();
}

MaterialVariant MaterialService::GetVariantByText(std::string type, std::string value)
{
    try {
        if (type == "INT") {
            return std::stoi(value);
        }
        else if (type == "FLOAT") {
            return std::stof(value);
        }
        else if (type == "BOOL") {
            return value == "1" || value == "true";
        }
        else if (type == "VECTOR2") {
            auto components = SplitString(value, ';');
            if (components.size() >= 2) {
                return Math::Vector2(
                    std::stof(components[0]),
                    std::stof(components[1])
                );
            }
        }
        else if (type == "VECTOR3") {
            auto components = SplitString(value, ';');
            if (components.size() >= 3) {
                return Math::Vector3(
                    std::stof(components[0]),
                    std::stof(components[1]),
                    std::stof(components[2])
                );
            }
        }
        else if (type == "VECTOR4") {
            auto components = SplitString(value, ';');
            if (components.size() >= 4) {
                return Math::Vector4(
                    std::stof(components[0]),
                    std::stof(components[1]),
                    std::stof(components[2]),
                    std::stof(components[3])
                );
            }
        }
        else if (type == "MATRIX4X4") {
            auto components = SplitString(value, ';');
            if (components.size() >= 16) {
                Math::Matrix4x4 mat;
                for (int i = 0; i < 4; i++) {
                    for (int u = 0; u < 4; u++)
                        mat.values[i][u] = std::stof(components[i + u]);
                }
                return mat;
            }
        }
        else if (type == "TEXTURE") {
            ServiceLocator::Get<Logger>()->Log("Add texture load in material.",
                Logger::Level::Warning);
        }
        throw std::runtime_error("Unknown type or invalid format");
    }
    catch (const std::exception& e) {
        ServiceLocator::Get<Logger>()->Log(
            "Failed to parse uniform value: " + std::string(e.what()) +
            " (Type: " + type + ", Value: " + value + ")",
            Logger::Level::Error
        );
        return {}; // Возвращаем default-конструированный вариант
    }
}



void MaterialService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> MaterialService::GetComponentFields(void* component)
{
    std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
    if (component == nullptr) return result;

    Material* material = static_cast<Material*>(component);

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
    Material mat;

    for (auto [fieldName, fieldValue] : fields)
    {
        if (fieldName == "Path") {
            if (auto wu = ECS::ESDL::ValueParser::GetAs<std::string>(fieldValue)) {
                mat = CreateMaterial(*wu);
            }
            else {
                ServiceLocator::Get<Logger>()->Log("Material's deserialization error: value's type is not string. Entity " + std::to_string(id) + ".", Logger::Level::Error);
            }
        }
        else //Это юниформы
        {
            if (auto wu = ECS::ESDL::ValueParser::GetAs<std::vector<float>>(fieldValue)) {
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
            else if (auto wu = ECS::ESDL::ValueParser::GetAs<float>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::ValueParser::GetAs<int>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::ValueParser::GetAs<bool>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else if (auto wu = ECS::ESDL::ValueParser::GetAs<std::string>(fieldValue)) {
                mat.uniforms[fieldName].data = *wu;
            }
            else {
                ServiceLocator::Get<Logger>()->Log("Material's deserialization error: undefined type. Entity " + std::to_string(id) + ".", Logger::Level::Error);
            }
        }
    }
    if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<Material>(id, mat);
    else
    {
        ServiceLocator::Get<Logger>()->Log("Material's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
    }

}
