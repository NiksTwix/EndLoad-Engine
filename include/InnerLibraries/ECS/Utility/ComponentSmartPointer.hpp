#pragma once
#include "ECS\Utility\Definitions.hpp"


namespace ECS
{
    class Registry;

    class RAWCSP
    {
        //RAWSCP - Raw ComponentSmartPointer - ����� ��������� �� ���������. ����� ��� ������������, �� ���� �����-��������
    protected:
        CSP_ID id = INVALID_ID;
        virtual void Register() {}
        virtual void Unregister() {}

        static CSP_ID GetNextID()
        {
            static CSP_ID next_id = 0;
            return next_id++;
        }

    public:
        virtual void Refresh() {}

        CSP_ID GetID() { return id; }
    };


    template<typename T>
    class ComponentSmartPointer : public RAWCSP
    {
    private:
        T* cached_ptr = nullptr;
        Registry* registry = nullptr;
        EntityID entity = INVALID_ID;

        void RefreshCache()
        {
            if (registry && registry->Has<T>(entity)) {
                cached_ptr = &registry->Get<T>(entity);
            }
            else {
                cached_ptr = nullptr;
            }
        }

        void Register() override
        {
            if (!registry || entity == INVALID_ID) return;
            id = GetNextID();
            registry->RegCSP(*this);
        }
        void Unregister() override
        {
            if (!registry|| entity == INVALID_ID) return;
            registry->UnRegCSP(*this);
        }
    public:
        ComponentSmartPointer() = default;


        EntityID GetEntityID() { return entity; }


        ComponentSmartPointer(Registry& reg, EntityID ent)
            : registry(&reg), entity(ent)
        {
            RefreshCache();
            Register();
        }
        // ����������� ����������� � ����������������
        ComponentSmartPointer(const ComponentSmartPointer& other)
            : registry(other.registry),
            entity(other.entity),
            cached_ptr(other.cached_ptr)
        {
            if (registry && entity != INVALID_ID) {
                Register();  // ������� ����� ����������� � �������
            }
        }

        // �������� ������������ � �������
        ComponentSmartPointer& operator=(const ComponentSmartPointer& other) {
            if (this != &other) {
                registry = other.registry;
                entity = other.entity;
                cached_ptr = other.cached_ptr;
                if (registry && entity != INVALID_ID) {
                    Register();
                }
            }
            return *this;
        }

        // �������� ����������
        explicit operator bool() const
        {
            return cached_ptr != nullptr;
        }

        // ������ � ����������
        T* operator->()
        {
            return cached_ptr;
        }

        const T* operator->() const
        {
            return cached_ptr;
        }

        T& operator*()
        {
            return *cached_ptr;
        }

        const T& operator*() const
        {
            return *cached_ptr;
        }

        T* Get()
        {
            if (registry && registry->Has<T>(entity)) return cached_ptr;
            else return nullptr;
        }


        // ������������ � ����� ��������
        void Rebind(EntityID new_entity)
        {
            entity = new_entity;
            RefreshCache();
        }

        // ���������� ���� (�������� ��� ��������� Registry)
        void Refresh() override
        {
            RefreshCache();
        }

        ~ComponentSmartPointer()
        {
            Unregister();
        }
    };
}