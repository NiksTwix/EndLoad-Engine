#pragma once
#include <ECS/Utility/Definitions.hpp>
#include <typeindex>


namespace ECS {
    class Event {
    public:
        virtual ~Event() = default;
        EntityID entity; // ������������ ���� ��� ���� ECS-�������
    };
}