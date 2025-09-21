#pragma once
#include "../Utility/Definitions.hpp"
#include <typeindex>


namespace ECS {
    class Event {
    public:
        virtual ~Event() = default;
        Event() = default;
        EntityID entity; // Обязательное поле для всех ECS-событий
    };
}