#pragma once
namespace ECS
{
    class Registry;

    template<typename T>
    class ComponentSmartPointer {
    private:
        Registry* registry = nullptr;
        EntityID entity = INVALID_ID;
        mutable T* cached_ptr = nullptr; // mutable для const-корректности

    public:
        ComponentSmartPointer(Registry& reg, EntityID ent)
            : registry(&reg), entity(ent) {
        }
        ComponentSmartPointer() 
        {
        
        }

        void Rebuild(Registry& reg, EntityID ent) 
        {
            registry = &reg;
            entity = ent;
        }

        EntityID GetEntityID() 
        {
            return entity;
        }

        // Явное обновление кеша
        void Refresh() const {
            cached_ptr = registry->template TryGet<T>(entity);
        }

        // Безопасный доступ
        T* Get() const {
            if (!registry || entity == INVALID_ID) return nullptr;
            auto* pool = registry->template GetPool<T>(); // Прямой доступ без TryGet
            return pool && pool->Has(entity) ? &pool->GetTyped(entity) : nullptr;
        }

        // Операторы для удобства
        T* operator->() const {
            T* ptr = Get();
            assert(ptr && "Dereferencing null ComponentSmartPointer");
            return ptr;
        }

        T& operator*() const {
            T* ptr = Get();
            assert(ptr && "Dereferencing null ComponentSmartPointer");
            return *ptr;
        }

        explicit operator bool() const {
            return Get() != nullptr;
        }
    };
}