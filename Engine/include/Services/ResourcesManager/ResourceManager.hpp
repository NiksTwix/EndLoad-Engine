#pragma once
#include <Core\IServices.hpp>
#include <Systems\Graphics\Windows\Window.hpp>
#include <Resources\IResource.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>

namespace Resources
{

	constexpr std::chrono::minutes MaxUnusedPeriod = std::chrono::minutes(5);
	constexpr Definitions::uint MinUsedCounts = 5;

	struct ResourceCashCell
	{
		std::shared_ptr<IResource> m_resource;
		size_t m_useCount = 0;
		Diagnostics::stclock::time_point m_lastUsedTime = Diagnostics::stclock::now();
		bool m_isPinned = false; //If resource is very important

		void UpdateData() {
			m_lastUsedTime = Diagnostics::stclock::now();
			m_useCount++;
		}
	};
	struct ResourcesFrame {
	private:
		std::unordered_map<ResourceID, ResourceCashCell> m_staticResources;
		std::unordered_map<std::string, ResourceCashCell> m_runtimeResources;	//Name-resource

		Windows::WindowID m_ownerWindow = Definitions::InvalidID;

		ResourceCashCell* GetStaticResource(ResourceID id)
		{
			return m_staticResources.count(id) > 0 ? &m_staticResources[id] : nullptr;
		}
		ResourceCashCell* GetRuntimeResource(const std::string& name)
		{
			return m_runtimeResources.count(name) > 0 ? &m_runtimeResources[name] : nullptr;
		}
	public:

		ResourcesFrame() = default;

		ResourcesFrame(Windows::WindowID owner_window) 
		{
			m_ownerWindow = owner_window;
			m_staticResources = std::unordered_map<ResourceID, ResourceCashCell>();
			m_runtimeResources = std::unordered_map<std::string, ResourceCashCell>();
		}
		ResourceID AddStaticResource(std::shared_ptr<IResource> resource)
		{
			if (resource == nullptr) return Definitions::InvalidID;
			m_staticResources[resource->GetID()] = ResourceCashCell(resource);
			return resource->GetID();
		}
		void AddRuntimeResource(const std::string& name,std::shared_ptr<IResource> resource)
		{
			m_runtimeResources[name] = ResourceCashCell(resource);
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResource(ResourceID id) {
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			auto resource = GetStaticResource(id); // Базовый метод
			if (!resource) return nullptr;
			resource->UpdateData();
			// Дополнительная проверка типа
			auto casted = std::dynamic_pointer_cast<Resource>(resource->m_resource);
			if (!casted) {
				Diagnostics::Logger::Get().SendMessage("Type mismatch in GetResource", Diagnostics::MessageType::Error);
				return nullptr;
			}
			return casted;
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResourceI(ResourceID id, bool is_runtime = false)	//I - Init
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			auto result = GetResource<Resource>(id, is_runtime);
			if (result) 
			{
				if (ResourceState rs = result->GetState(); rs == ResourceState::NeedReinit || rs == ResourceState::Loaded)
				{
					result->Init();
				}
			}
			return result;
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResource(const std::string& name) {
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			auto resource = GetRuntimeResource(name); // Базовый метод
			if (!resource) return nullptr;
			resource->UpdateData();
			// Дополнительная проверка типа
			auto casted = std::dynamic_pointer_cast<Resource>(resource->m_resource);
			if (!casted) {
				Diagnostics::Logger::Get().SendMessage("Type mismatch in GetResource", Diagnostics::MessageType::Error);
				return nullptr;
			}
			return casted;
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResourceI(const std::string& name)	//I - Init
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			auto result = GetResource<Resource>(name);
			if (result)
			{
				if (ResourceState rs = result->GetState(); rs == ResourceState::NeedReinit || rs == ResourceState::Loaded)
				{
					result->Init();
				}
			}
			return result;
		}
		template<typename Resource>
		ResourceID Load(const std::string& path) {
			auto resource = std::make_shared<Resource>();
			if (resource->Load(path)) {
				return AddStaticResource(resource);;
			}
			return Definitions::InvalidID;
		}

		bool Exists(ResourceID id);
		bool Exists(const std::string& name);

		void DeleteResource(ResourceID id);
		void DeleteResource(const std::string& name);

		void ClearStaticResources();

		void ClearRuntimeResources();

		void ClearAllResources();

		~ResourcesFrame() 
		{
			ClearAllResources();
		}

		void CleanupUnusedResources();
	};

	class ResourceManager : public Core::IService 
	{
	private:
		Windows::WindowID m_activeWindow = Definitions::InvalidID;
	
		std::unordered_map<Windows::WindowID, std::shared_ptr<ResourcesFrame>> m_resources;	//Window - resources
		
	public:
		
		template<typename Resource>
		std::shared_ptr<Resource> LoadResource(const std::string& path) 
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			std::shared_ptr<Resource> resource = std::make_shared<Resource>();

			if (!resource->Load(path)) 
			{
				Diagnostics::Logger::Get().SendMessage("(ResourceManager) Loaging of resource on path \"" + path + "\" failed.", Diagnostics::MessageType::Error);
				return nullptr;
			}
			return resource;
		}
		template<typename Resource>
		std::shared_ptr<Resource> CreateResource()//Creates static resource
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			std::shared_ptr<Resource> resource = std::make_shared<Resource>();
			return resource;
		}

		ResourceID AttachResourceToFrame(std::shared_ptr<IResource> resource);

		ResourcesFrame* GetActiveFrame();

		void SetActiveWindow(Windows::WindowID new_active_window);
		void ClearWindowData(Windows::WindowID id);
		void ClearWindowCache(Windows::WindowID id);
		void Init() override;
		void Shutdown() override;

		void AttachStaticResources(const std::vector<std::shared_ptr<Resources::IResource>>& resources);
	};
} 