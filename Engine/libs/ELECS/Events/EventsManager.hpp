#pragma once
#include "Events.hpp"
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace ECS {
    using EventHandler = std::function<void(const Event&)>;
    using HandlerID = size_t;

    struct Handler {
        HandlerID id;
        EntityID entity;
        std::type_index eventType;
        EventHandler function;
    };

    class EventsManager {
    private:
        std::unordered_map<std::type_index, std::unordered_set<HandlerID>> m_typeToHandlers;
        std::unordered_map<HandlerID, std::shared_ptr<Handler>> m_allHandlers;
        std::unordered_map<EntityID, std::unordered_set<HandlerID>> m_entityToHandlers;
        std::atomic<HandlerID> m_nextID{ 0 };

        std::mutex m_mutex;

        void RemoveHandler(HandlerID id) {
            if (auto it = m_allHandlers.find(id); it != m_allHandlers.end()) {
                auto& handler = *it->second;
                m_typeToHandlers[handler.eventType].erase(id);
                m_entityToHandlers[handler.entity].erase(id);
                m_allHandlers.erase(id);
            }
        }

    public:
        // Подписка на событие
        template<typename T>
        HandlerID Subscribe(EntityID entity, EventHandler handler) {
            static_assert(std::is_base_of_v<Event, T>, "Event must inherit from Event");
            std::lock_guard lock(m_mutex);
            HandlerID id = m_nextID++;
            auto handlerPtr = std::make_shared<Handler>(Handler{ id, entity, typeid(T), handler });

            m_allHandlers[id] = handlerPtr;
            m_typeToHandlers[typeid(T)].insert(id);
            m_entityToHandlers[entity].insert(id);
            return id;
        }

        // Отправка события
        template<typename T>
        void Emit(const T& event) {
            static_assert(std::is_base_of_v<Event, T>, "Event must inherit from Event");
            auto type = typeid(T);
            if (auto it = m_typeToHandlers.find(type); it != m_typeToHandlers.end()) {
                for (HandlerID id : it->second) {
                    m_allHandlers[id]->function(event);
                }
            }
        }

        // Отправка события для конкретной сущности
        template<typename T>
        void EmitForEntity(EntityID entity, const T& event) {
            static_assert(std::is_base_of_v<Event, T>, "Event must inherit from Event");
            auto type = typeid(T);
            if (auto it = m_entityToHandlers.find(entity); it != m_entityToHandlers.end()) {
                for (HandlerID id : it->second) {
                    if (m_allHandlers[id]->eventType == type) {
                        m_allHandlers[id]->function(event);
                    }
                }
            }
        }

        // Отписка по ID
        bool Unsubscribe(HandlerID id) {
            std::lock_guard lock(m_mutex);
            if (m_allHandlers.count(id)) {
                RemoveHandler(id);
                return true;
            }
            return false;
        }

        // Отписка всех обработчиков сущности
        void UnsubscribeAllForEntity(EntityID entity) {
            std::lock_guard lock(m_mutex);
            if (auto it = m_entityToHandlers.find(entity); it != m_entityToHandlers.end()) {
                auto ids = std::move(it->second); // Копируем IDs, чтобы избежать итерационной инвалидации
                m_entityToHandlers.erase(it);
                for (HandlerID id : ids) {
                    RemoveHandler(id);
                }
            }
        }

        void InvokeHandler(HandlerID handler, const Event& event) 
        {
            std::lock_guard lock(m_mutex);
            if (m_allHandlers.count(handler) == 0);
            m_allHandlers[handler]->function(event);
        }

    };
}

