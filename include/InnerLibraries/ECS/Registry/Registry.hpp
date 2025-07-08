#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <shared_mutex>
#include "ECS\Utility\Definitions.hpp"
#include "ECS\ComponentPool\TypedComponentPool.hpp"
#include "ECS\Utility\ComponentSmartPointer.hpp"


namespace ECS
{
    class Registry
    {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<ComponentPool>> pools;
        std::unordered_map<CSP_ID, std::reference_wrapper<RAWCSP>> smart_ptrs; 

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
            for (auto& [_, pool] : pools) {  // Используем _ для игнорирования ключа
                pool->Remove(entity);
            }
        }

        std::unordered_map<std::type_index, void*> GetAllComponents(EntityID id)   //TODO Добавить мьютекс
        {
            std::unordered_map<std::type_index, void*> components;
            for (auto& [_, pool] : pools) {
                if (pool->Has(id)) {
                    components[pool->GetComponentType()] = pool->Get(id);
                }
            }
            return components;
        }


        // Быстрая проверка наличия компонента

        template<typename T>
        void Add(EntityID entity, const T& component) 
        {
            std::unique_lock lock(mutex);
            GetPool<T>()->Add(entity, component);
            RefreshCSP();
            
        }

        template<typename T>
        bool Has(EntityID entity) const
        {
            auto it = pools.find(typeid(T));
            return it != pools.end() && it->second->Has(entity);
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
            if (it == pools.end()) return false;
            RefreshCSP();
            return static_cast<TypedComponentPool<T>*>(it->second.get())->Remove(entity);
        }

        template<typename T>
        ComponentSmartPointer<T> GetSmartPointer(EntityID entity)
        {
            return ComponentSmartPointer<T>(*this, entity);
        }

        void RegCSP(RAWCSP& csp)     //FOR ComponentSmartPointer
        {
            if (csp.GetID() == INVALID_ID) return;

            smart_ptrs.emplace(csp.GetID(),std::ref(csp));
        }
        void UnRegCSP(RAWCSP& csp)                //FOR ComponentSmartPointer
        {
            if (smart_ptrs.count(csp.GetID()) == 0) return;

            smart_ptrs.erase(csp.GetID());
        }

        void RefreshCSP()                           //FOR ComponentSmartPointer
        {

            for (auto& [key, csp] : smart_ptrs) 
            {
                RAWCSP& csp_ = csp;
                csp_.Refresh();
            }
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


    };
}