#pragma once 
#include <variant>
#include <unordered_map>
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <Services\Resources\Graphics\Shader.hpp>
#include <vector>
#include <ECS\ECS.hpp>

using MaterialVariant = std::variant<int, float, bool, Math::Vector2, Math::Vector3, Math::Vector4, Math::Matrix4x4, std::string>;


class SceneContext;

/*
Окей, то есть я создаю компонент Material, который хранит в себе айди сущности и хеш-карту с юниформами (использовать стоит std::variant), 
дальше в методе рендеринга вызываю MaterialService, 
который, получая структуру материала, считывает данные и отправляет в шейдер
*/

struct UniformValue {
    MaterialVariant data;
};


struct Material
{
    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, UniformValue> uniforms;
};


class MaterialService : public ECS::IComponentService
{
//private:
//    std::unordered_map<std::string, std::shared_ptr<GLShader>> shaders;
public:

    MaterialService();

    virtual void Init() override;  // Опционально
    virtual void Shutdown() override;

    Material CreateMaterial(const std::string& path_to_shader);

    void Bind(Material& material) const;
    void Unbind(Material& material) const;
    void Update(ECS::EntitySpace* eSpace) override;

    std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
    void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;


private:
    MaterialVariant GetVariantByText(std::string type, std::string value);

    
};