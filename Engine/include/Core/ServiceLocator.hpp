#pragma once
#include "IServices.hpp"
#include <typeindex>
#include <mutex>

namespace Core 
{
    class ServiceLocator
    {
    public:
        ServiceLocator(const ServiceLocator&) = delete;
        ServiceLocator& operator=(const ServiceLocator&) = delete;

        template<class T>
        static T* Get()
        {
            static_assert(std::is_base_of_v<IService, T>, "T must inherit from IService");

            if constexpr (std::is_base_of_v<ISystem, T>) {
                return GetSystem<T>();
            }
            else if constexpr (std::is_base_of_v<IService, T>) {
                return GetServicePtr<T>();
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
            else if constexpr (std::is_base_of_v<IService, T>) {
                return GetServicePtr<T>() != nullptr;
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
            //auto logger = Get<Logger>();
            auto list_ = GetSystemList();


            for (int i = list_.size() - 1; i >= 0; i--)  
            {
                list_[i]->Shutdown();
            }

            for (auto& [_, service] : GetServices())
            {
                service->Shutdown();
            }
            //logger->Log("(ServiceLocator): All services shutdown.", Logger::Level::Success);
            GetSystems().clear();
            GetServices().clear();
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

        static std::unordered_map<std::type_index, std::unique_ptr<ISystem>>& GetSystems() {
            static auto systems = std::unordered_map<std::type_index, std::unique_ptr<ISystem>>();
            return systems; 
        }

        static std::unordered_map<std::type_index, std::unique_ptr<IService>>& GetServices() {
            static auto services = std::unordered_map<std::type_index, std::unique_ptr<IService>>();
            return services;
        }

        static std::mutex& GetMutex() {
            static std::mutex mutex;
            return mutex;
        }

        static bool& IsDirty() {
            static bool isDirty;
            return isDirty;
        }

        template<typename T>
        static T* GetSystem() {
            static_assert(std::is_base_of_v<ISystem, T>, "T must be ISystem");
            auto ptr = GetSystemPtr<T>();
            return static_cast<T*>(ptr);
        }

        template<typename T>
        static T* GetHelperService() {
            static_assert(std::is_base_of_v<IHelperService, T>, "T must be IHelperService");
            auto ptr = GetServicePtr<T>();
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
        static IService* GetServicePtr() {
            std::lock_guard<std::mutex> lock(GetMutex());
            auto& services = GetServices();
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
            else if constexpr (std::is_base_of_v<IService, T>) {
                GetServices()[type] = std::unique_ptr<IService>(static_cast<IService*>(service.release()));
            }
        }

        template<typename T>
        static void UnregisterService() {
            std::lock_guard<std::mutex> lock(GetMutex());
            const std::type_index type(typeid(T));

            if constexpr (std::is_base_of_v<ISystem, T>) {
                GetSystems().erase(type);
            }
            else if constexpr (std::is_base_of_v<IService, T>) {
                GetServices().erase(type);
            }
        }
    };
}