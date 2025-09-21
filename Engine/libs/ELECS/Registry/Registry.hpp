#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include "..\Utility\Definitions.hpp"
#include "..\ComponentPool\TypedComponentPool.hpp"
#include "..\Utility\ComponentSmartPointer.hpp"


namespace ECS
{
    struct ECTT { //Entity Component Types Table
    private:
        size_t nextBitIndex = 0;
        std::unordered_map<std::type_index, size_t> typeToBit;
        std::vector<const std::type_index*> bitToType;
        std::unordered_map<EntityID, std::bitset<MAX_COMPONENTS>> entityMasks;
        // Хранилище для стабильных type_index
        std::unordered_map<std::type_index, std::unique_ptr<std::type_index>> typeStorage;
    public:
        template<typename T>
        size_t RegisterType() {
            if (auto it = typeToBit.find(typeid(T)); it != typeToBit.end()) {
                return it->second;
            }

            if (nextBitIndex >= MAX_COMPONENTS) {
                throw std::runtime_error("Max component types exceeded");
            }

            size_t bit = nextBitIndex++;

            const auto atype = std::type_index(typeid(T));

            typeToBit[atype] = bit;
            typeStorage[atype] = std::make_unique<std::type_index>(atype);
            if (bit >= bitToType.size()) {
                bitToType.resize(bit + 1);
            }
            bitToType[bit] = typeStorage[atype].get();

            return bit;
        }
        template<typename T>
        void AddComponent(EntityID entity) {
            size_t bit = RegisterType<T>();
            entityMasks[entity].set(bit);
        }

        
        template<typename T>
        void RemoveComponent(EntityID entity) {
            if (auto it = typeToBit.find(typeid(T)); it != typeToBit.end()) {
                entityMasks[entity].reset(it->second);
            }
        }
        template<typename T>
        bool HasComponent(EntityID entity) const {
            if (!entityMasks.count(entity))return false;
            if (auto it = typeToBit.find(typeid(T)); it != typeToBit.end()) {
                return entityMasks.at(entity).test(it->second);
            }
            return false;
        }

        std::vector<std::type_index> GetComponentTypes(EntityID entity) const {
            std::vector<std::type_index> result;
            if (auto it = entityMasks.find(entity); it != entityMasks.end()) {
                const auto& mask = it->second;
                for (size_t i = 0; i < nextBitIndex; ++i) {
                    if (mask.test(i)) {
                        result.push_back(*bitToType[i]);
                    }
                }
            }
            return result;
        }

        void RemoveAllComponents(EntityID entity) 
        {
            entityMasks[entity] = std::bitset<MAX_COMPONENTS>();
        }

    };
    class Registry
    {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<ComponentPool>> pools;

        ECTT ectt;

        mutable std::shared_mutex mutex;

        template<typename T>
        TypedComponentPool<T>* CreatePool()
        {
            auto pool = std::make_unique<TypedComponentPool<T>>();
            auto* ptr = pool.get();
            pools[typeid(T)] = std::move(pool);
            return ptr;
        }

    public:
        template<typename T>
        TypedComponentPool<T>* GetPool()
        {
            auto it = pools.find(typeid(T));
            return it != pools.end()
                ? static_cast<TypedComponentPool<T>*>(it->second.get())
                : CreatePool<T>();
        }

        void UpdateAll()
        {
            for (auto& [_, pool] : pools) {
                pool->Update();
            }
        }

        void RemoveAll(EntityID entity) 
        {
            std::unique_lock lock(mutex);
            //for (auto& [_, pool] : pools) {  // Используем _ для игнорирования ключа
            //    pool->Remove(entity);
            //}
            auto types = ectt.GetComponentTypes(entity);
            for (auto& type : types)
            {
                pools[type]->Remove(entity);
            }
            ectt.RemoveAllComponents(entity);
        }

        std::unordered_map<std::type_index, void*> GetAllComponents(EntityID entity)   
        {
            std::unordered_map<std::type_index, void*> components;
            //for (auto& [_, pool] : pools) {
            //    if (pool->Has(entity)) {
            //        components[pool->GetComponentType()] = pool->Get(entity);
            //    }
            //}

            auto types = ectt.GetComponentTypes(entity);

            for (auto& type : types) 
            {
                components[type] = pools[type]->Get(entity);
            }

            return components;
        }


        // Быстрая проверка наличия компонента

        template<typename T>
        void Add(EntityID entity, const T& component) 
        {
            std::unique_lock lock(mutex);
            if (ectt.HasComponent<T>(entity)) 
            {
                GetPool<T>()->Replace(entity, component);
            }
            else GetPool<T>()->Add(entity, component);
            ectt.AddComponent<T>(entity);
        }
        template<typename T>
        bool Has(EntityID entity) const
        {
            //auto it = pools.find(typeid(T));
            //return it != pools.end() && it->second->Has(entity);
            return ectt.HasComponent<T>(entity);
        }

        // Быстрое получение компонента
        template<typename T>
        T& Get(EntityID entity)
        {
            auto it = pools.find(typeid(T));
            if (it == pools.end()) throw std::runtime_error("Component pool not found");
            return static_cast<TypedComponentPool<T>*>(it->second.get())->GetTyped(entity);
        }

        // Безопасное получение
        template<typename T>
        T* TryGet(EntityID entity)
        {
            auto it = pools.find(typeid(T));
            if (it == pools.end()) return nullptr;
            auto* pool = static_cast<TypedComponentPool<T>*>(it->second.get());
            return pool->Has(entity) ? &pool->GetTyped(entity) : nullptr;
        }

        // Удаление компонента
        template<typename T>
        bool Remove(EntityID entity)
        {
            std::unique_lock lock(mutex);
            auto it = pools.find(typeid(T));
            ectt.RemoveComponent<T>(entity);
            if (it == pools.end()) return false;
            return static_cast<TypedComponentPool<T>*>(it->second.get())->Remove(entity);
        }

        template<typename T>
        std::shared_ptr<ComponentSmartPointer<T>> GetSmartPointer(EntityID entity) {
            return std::make_shared<ComponentSmartPointer<T>>(*this, entity);
        }

        //Serialization help


        bool HasByTypeIndex(std::type_index type, EntityID entity) 
        {
            auto it = pools.find(type);
            return it != pools.end() && pools[type]->Has(entity);
        }

        void* GetByTypeIndex(std::type_index type, EntityID entity) //Returns raw data
        {
            auto it = pools.find(type);
            return it != pools.end() ? pools[type]->Get(entity): nullptr;
        }


        std::vector<std::type_index> GetComponentsTypes(ECS::EntityID entity)
        {
            return ectt.GetComponentTypes(entity);
        }
    };
}