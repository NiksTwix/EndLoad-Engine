#pragma once
#include <Math\Vector2.hpp>
#include <Math\Vector4.hpp>
#include <Components\Basic\TransformComponent.hpp>
#include <ECS\ECS.hpp>

struct UIElementComponent 
{
	Math::Vector2 Size;
	ECS::EntityID CameraID;		//�������� � ������
	Math::Vector4 Color;
};


class UIElementService : public ECS::IComponentService 
{
public:
	// ������������ ����� IComponentService
	void Init() override;
	void Update(ECS::EntitySpace* eSpace) override;
	void Shutdown() override;
	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields) override;

	void UpdateCameraID();		//��������� id ����� � ��������� ����������

	void UpdateUIGT(); //��������� ���������� ������������� ui element

	void UpdateComponent(ECS::EntityID entity) override;


};

