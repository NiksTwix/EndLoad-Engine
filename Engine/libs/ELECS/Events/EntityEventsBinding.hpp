#pragma once
#include <ECS\Utility\Definitions.hpp>
#include <ECS\Events\EventsManager.hpp>

namespace ECS 
{
    class EntityEventBinding {
    public:
        EntityEventBinding(EntityID entity, EventsManager& events)
            : entity(entity), events(events) {
        }

        template<typename T>
        void AddHandler(std::function<void(const T&)> handler) {
            events.Subscribe<T>(entity,[this, handler](const T& e) {
                if (e.entity == this->entity) {  // Фильтрация по сущности
                    handler(e);
                }
                });
        }

    private:
        EntityID entity;
        EventsManager& events;
    };
}
