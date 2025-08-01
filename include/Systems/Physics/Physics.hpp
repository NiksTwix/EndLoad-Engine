#pragma once
#include <Core\ISystem.hpp>



class Physics : public ISystem 
{
public:
    // ��������� ����������� (���� �����)
    Physics(Physics&&) = default;
    Physics& operator=(Physics&&) = default;

    Physics();
    ~Physics();

    virtual void Init() override;
    virtual void Shutdown() override;

    virtual SystemPriority GetPriority() override { return SystemPriority::PHYSICS; }

    void Update() override;

private:
    void BasicUpdate(); //��������� ������� ���������� � �������� (�������, ���������� ��������� � ��)

};