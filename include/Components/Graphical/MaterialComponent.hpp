#pragma once 
#include <variant>
#include <unordered_map>
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <Resources\Shader.hpp>
#include <vector>
#include <ECS\ECS.hpp>

using MaterialVariant = std::variant<int, float, bool, Math::Vector2, Math::Vector3, Math::Vector4, Math::Matrix4x4, std::string>;


class SceneContext;

/*
����, �� ���� � ������ ��������� Material, ������� ������ � ���� ���� �������� � ���-����� � ���������� (������������ ����� std::variant), 
������ � ������ ���������� ������� MaterialService, 
�������, ������� ��������� ���������, ��������� ������ � ���������� � ������
*/

struct UniformValue {
    MaterialVariant data;
};


struct MaterialComponent
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

    virtual void Init() override;  // �����������
    virtual void Shutdown() override;

    MaterialComponent CreateMaterial(const std::string& path_to_shader);

    void Bind(MaterialComponent& material) const;
    void Unbind(MaterialComponent& material) const;
    void Update(ECS::EntitySpace* eSpace) override;

    std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
    void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;
    
};