#pragma once
#include <tuple>
#include <functional>
#include "..\Registry\Registry.hpp"

namespace ECS
{
    template<typename... Components>
    class View
    {
    private:
        Registry& registry;
        std::tuple<TypedComponentPool<Components>*...> pools;

        template<size_t I = 0>
        bool HasAllComponents(EntityID entity) const
        {
            if constexpr (I < sizeof...(Components))
            {
                return std::get<I>(pools)->Has(entity) && HasAllComponents<I + 1>(entity);
            }
            return true;
        }

    public:
        explicit View(Registry& reg)
            : registry(reg)
            , pools((reg.GetPool<Components>())...)
        {}

        // Итерация с доступом на чтение
        template<typename Func>
        void Each(Func func) const
        {
            auto& firstPool = *std::get<0>(pools);
            for (EntityID entity : firstPool.GetAllEntities())
            {
                if (HasAllComponents(entity))
                {
                    func(entity, std::get<TypedComponentPool<Components>*>(pools)->GetTyped(entity)...);
                }
            }
        }

        // Итерация с доступом на запись
        template<typename Func>
        void EachForWrite(Func func)
        {
            auto& firstPool = *std::get<0>(pools);
            for (EntityID entity : firstPool.GetAllEntities())
            {
                if (HasAllComponents(entity))
                {
                    func(entity, std::get<TypedComponentPool<Components>*>(pools)->GetFromWritable(entity)...);
                }
            }
        }
    };
}