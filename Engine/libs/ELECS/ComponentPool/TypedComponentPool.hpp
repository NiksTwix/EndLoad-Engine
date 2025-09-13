#pragma once
#include "ECS\ComponentPool\ComponentPool.hpp"
#include "ECS\SparseSet\SparseSet.hpp"
#include <shared_mutex>
#include <type_traits>
#include <bitset>

namespace ECS
{
    template<typename T>
    class TypedComponentPool : public ComponentPool
    {
    protected:
        SparseSet<T> readable;
        std::vector<T> delta_buffer;
        std::vector<EntityID> dirty_entities;
        std::bitset<MAX_ENTITIES> dirty_flags; // Замените на динамический bitset при необходимости
        mutable std::shared_mutex m_mutex;
    public: 
        bool Has(EntityID entity) const override
        {
            return readable.Has(entity);
        }

        T& GetFromWritable(EntityID entity)
        {
            std::unique_lock lock(m_mutex);
            if (!dirty_flags.test(entity)) {
                dirty_flags.set(entity);
                dirty_entities.push_back(entity);
                delta_buffer.push_back(readable.Get(entity));
            }
            return delta_buffer.back();
        }

        void Update() override
        {
            std::unique_lock lock(m_mutex);
            for (EntityID entity : dirty_entities) {
                readable.Replace(entity, delta_buffer[dirty_flags[entity]]);
            }
            dirty_entities.clear();
            delta_buffer.clear();
            dirty_flags.reset();
        }

        void Add(EntityID entity, const T& component)
        {
            std::lock_guard<std::shared_mutex> lock(m_mutex);
            // Безопасная проверка типа
            readable.Add(entity, component);
        }
        void Replace(EntityID entity, const T& component)
        {
            std::lock_guard<std::shared_mutex> lock(m_mutex);
            // Безопасная проверка типа
            readable.Replace(entity, component);
        }

        bool Add(EntityID entity, const void* component) override
        {
            if (!component) return false;
            std::lock_guard<std::shared_mutex> lock(m_mutex);
            // Безопасная проверка типа
            static_assert(!std::is_pointer_v<T>, "Component type cannot be pointer");
            try {
                readable.Add(entity, *static_cast<const T*>(component));
                return true;
            }
            catch (...) {
                return false;
            }
        }

        void* Get(EntityID entity) override {
            return &readable.Get(entity);
        }

        const void* Get(EntityID entity) const override {
            return &readable.Get(entity);
        }

        // Безопасные типизированные методы
        T& GetTyped(EntityID entity) {
            return readable.Get(entity);
        }

        const T& GetTyped(EntityID entity) const {
            return readable.Get(entity);
        }

        bool Remove(EntityID entity) override {
            std::lock_guard<std::shared_mutex> lock(m_mutex);
            return readable.Has(entity) ? (readable.Remove(entity), true) : false;
        }

        std::type_index GetComponentType() const override {
            return typeid(T);
        }

        // Фабричный метод для безопасного создания
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args)
        {
            std::lock_guard<std::shared_mutex> lock(m_mutex);
            // Проверка на поддержку конструкции T(Args...)
            static_assert(std::is_constructible_v<T, Args...>,
                "Component type must be constructible from these arguments");

            // Создаём компонент напрямую в хранилище
            return readable.Emplace(entity, std::forward<Args>(args)...);
        }

        std::vector<EntityID> GetAllEntities() override 
        {
            return readable.GetAllEntities();
        
        }

    };
}