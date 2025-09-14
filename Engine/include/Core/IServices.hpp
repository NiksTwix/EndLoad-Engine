#pragma once
#include "..\SpecialHeaders\Definitions.hpp"
#include <cstdint>
#include <algorithm>

namespace Core
{
    //IService
    class IService	//The basic class for systems and services 
    {
    public:
        IService() = default;
        virtual ~IService() = default;

        //Delete copying
        IService(const IService&) = delete;
        IService& operator=(const IService&) = delete;

        virtual void Init() { if (m_isValid) return; m_isValid = true; }
        virtual void Shutdown() { m_isValid = false; }

        bool IsValid() const { return m_isValid; }

    protected:
        bool m_isValid = false;
    };

    //ISystem

    using SystemModuleID = Definitions::identificator;

    class ISystemModule
    {
    private:
        static SystemModuleID GetNext() 
        {
            static SystemModuleID next_id = 0;
            return next_id++;
        }
        SystemModuleID m_id = Definitions::InvalidID;
    public:
        ISystemModule() 
        {
            m_id = GetNext();
        }
        SystemModuleID GetID() const { return m_id; }
        virtual ~ISystemModule() = default;
        virtual uint8_t GetPriority() const = 0;
        virtual void Update() = 0;      //Scene pointer and delta time he will get from services: SceneContext::GetRenderScene() and ProccessObserver::GetDeltaTime()) 
        virtual void UpdateResources() {}       //Update module resources (plane mesh for ui rendering and etc)
    };

    enum class SystemPriority {
        WINDOW_MANAGER = 0,  // WindowManager (Window's update in first queue is very important)
        INPUT = 1,  // InputSystem
        PHYSICS = 2,  // PhysicsSystem
        RENDER = 3,  // RenderSystem
        USER_SYSTEMS = 100 // User systems is not important for good engine work
    };

    class ISystem : public IService
    {
    protected:
        std::vector<std::shared_ptr<ISystemModule>> m_modules;  // System modules

        void ModulesSorting() 
        {
            std::sort(m_modules.begin(), m_modules.end(),
                [](const std::shared_ptr<ISystemModule> a, const std::shared_ptr<ISystemModule> b)
                {
                    return a->GetPriority() < b->GetPriority();
                }
            );
        }
    public:
        virtual void Update() {}

        virtual SystemPriority GetPriority() { return SystemPriority::USER_SYSTEMS; }

        void AddModule(std::shared_ptr<ISystemModule> module)
        {
            m_modules.push_back(module);
            ModulesSorting();
        }
        template<typename T, typename... Args>
        std::shared_ptr<T> AddModule(Args&&... args) {
            auto module = std::make_shared<T>(std::forward<Args>(args)...);
            m_modules.push_back(module);
            ModulesSorting();
            return module;
        }

        std::shared_ptr<ISystemModule> GetModule(SystemModuleID id) const
        {
            for (auto module_ : m_modules)
            {
                if (module_->GetID() == id) return module_;
            }
            return nullptr;
        }
        bool DeleteModule(SystemModuleID id)
        {
            for (size_t i = 0; i < m_modules.size(); i++)
            {
                if (m_modules[i]->GetID() == id)
                {
                    auto temp = m_modules.back();

                    m_modules[m_modules.size() - 1] = m_modules[i]; 

                    m_modules[i] = temp;    

                    m_modules.pop_back();
                    ModulesSorting();   //Resorting
                    return true;
                }
            }
            return false;
        }

        void UpdateModules() 
        {
            for (auto m_module : m_modules) {
                m_module->Update();
            }
        }
    };

    //IHelperService was cut as unnecessary
}