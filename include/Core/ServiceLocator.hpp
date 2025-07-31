#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <format>
#include <Core/IServices.hpp>
#include <Core/ISystem.hpp>
#include <Core/Logger.hpp>


class ServiceLocator
{
public:
    // Запрещаем копирование
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    template<class T>
    static T* Get()
    {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

        if constexpr (std::is_base_of_v<ISystem, T>) {
            return GetSystem<T>();
        }
        else if constexpr (std::is_base_of_v<IHelperService, T>) {
            return GetHelperService<T>();
        }
        else {
            return nullptr;
        }
    }

    template<class T, class Impl>
    static void Set()
    {
        static_assert(std::is_base_of_v<T, Impl>, "Impl must inherit from T");
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

        RegisterService<T>(std::make_unique<Impl>());
        IsDirty() = true;
    }

    template<class T>
    static bool Has()
    {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

        if constexpr (std::is_base_of_v<ISystem, T>) {
            return GetSystemPtr<T>() != nullptr;
        }
        else if constexpr (std::is_base_of_v<IHelperService, T>) {
            return GetHelperServicePtr<T>() != nullptr;
        }
        return false;
    }

    template<class T>
    static void Remove()
    {
        static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");
        UnregisterService<T>();
        IsDirty() = true;
    }

    static void Shutdown()
    {
        auto logger = Get<Logger>();
        auto list_ = GetSystemList();


        for (int i = list_.size() - 1; i >= 0; i--)  // Отключаются в обратном порядке приоритета
        {
            list_[i]->Shutdown();
        }

        for (auto& [_,service] : GetHelperServices())
        {
            service->Shutdown();   
        }
        logger->Log("(ServiceLocator): All services shutdown.", Logger::Level::Success);
        GetSystems().clear();
        GetHelperServices().clear();
    }

    static std::vector<ISystem*>& GetSystemList()
    {
        static std::vector<ISystem*> systems;

        if (!IsDirty()) return systems;
        systems.clear();

        std::lock_guard<std::mutex> lock(GetMutex());
        auto& systemMap = GetSystems();

        systems.reserve(systemMap.size());
        for (auto& pair : systemMap) {
            systems.push_back(pair.second.get());
        }

        std::sort(systems.begin(), systems.end(),
            [](ISystem* a, ISystem* b) {
                return a->GetPriority() < b->GetPriority();
            });

        IsDirty() = false;
        return systems;
    }

private:
    ServiceLocator() = default;
    ~ServiceLocator() = default;

    // Хранилища
    static std::unordered_map<std::type_index, std::unique_ptr<ISystem>>& GetSystems() {
        static auto* systems = new std::unordered_map<std::type_index, std::unique_ptr<ISystem>>();
        return *systems;
    }

    static std::unordered_map<std::type_index, std::unique_ptr<IHelperService>>& GetHelperServices() {
        static auto* services = new std::unordered_map<std::type_index, std::unique_ptr<IHelperService>>();
        return *services;
    }

    static std::mutex& GetMutex() {
        static std::mutex mutex;
        return mutex;
    }

    static bool& IsDirty() {
        static bool isDirty;
        return isDirty;
    }

    // Вспомогательные методы
    template<typename T>
    static T* GetSystem() {
        static_assert(std::is_base_of_v<ISystem, T>, "T must be ISystem");
        auto ptr = GetSystemPtr<T>();
        return static_cast<T*>(ptr);
    }

    template<typename T>
    static T* GetHelperService() {
        static_assert(std::is_base_of_v<IHelperService, T>, "T must be IHelperService");
        auto ptr = GetHelperServicePtr<T>();
        return static_cast<T*>(ptr);
    }

    template<typename T>
    static ISystem* GetSystemPtr() {
        std::lock_guard<std::mutex> lock(GetMutex());
        auto& systems = GetSystems();
        auto it = systems.find(typeid(T));
        return it != systems.end() ? it->second.get() : nullptr;
    }

    template<typename T>
    static IHelperService* GetHelperServicePtr() {
        std::lock_guard<std::mutex> lock(GetMutex());
        auto& services = GetHelperServices();
        auto it = services.find(typeid(T));
        return it != services.end() ? it->second.get() : nullptr;
    }

    template<typename T>
    static void RegisterService(std::unique_ptr<T> service) {
        static_assert(std::is_base_of_v<IService, T>, "Service must inherit from IService");

        std::lock_guard<std::mutex> lock(GetMutex());
        const std::type_index type(typeid(T));

        if constexpr (std::is_base_of_v<ISystem, T>) {
            GetSystems()[type] = std::unique_ptr<ISystem>(static_cast<ISystem*>(service.release()));
        }
        else if constexpr (std::is_base_of_v<IHelperService, T>) {
            GetHelperServices()[type] = std::unique_ptr<IHelperService>(static_cast<IHelperService*>(service.release()));
        }
    }

    template<typename T>
    static void UnregisterService() {
        std::lock_guard<std::mutex> lock(GetMutex());
        const std::type_index type(typeid(T));

        if constexpr (std::is_base_of_v<ISystem, T>) {
            GetSystems().erase(type);
        }
        else if constexpr (std::is_base_of_v<IHelperService, T>) {
            GetHelperServices().erase(type);
        }
    }
};
