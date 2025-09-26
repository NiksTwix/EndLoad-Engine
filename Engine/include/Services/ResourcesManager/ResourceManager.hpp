#pragma once
#include <Core\IServices.hpp>
#include <Systems\Graphics\Windows\Window.hpp>
#include <Resources\IResource.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>

namespace Resources
{

	constexpr std::chrono::minutes MaxUnusedPeriod = std::chrono::minutes(5);
	constexpr Definitions::uint MinUsedCounts = 5;

	using ResourceID = Definitions::identificator;

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
		std::unordered_map<ResourceID, ResourceCashCell> m_resources;
		std::unordered_map<ResourceID, ResourceCashCell> m_runTimeResources;	//Name-resource

		std::unordered_map<std::string, ResourceID> m_strId;	//StrIdentificator - ResourceID
		std::unordered_map<ResourceID, std::string> m_IdStr;		//ResourceID - StrIdentificator
		Windows::WindowID m_ownerWindow = Definitions::InvalidID;

		ResourceID GetNextID()
		{
			static ResourceID next_id = 0;
			return next_id++;
		}

	public:

		ResourcesFrame() = default;

		ResourcesFrame(Windows::WindowID owner_window) 
		{
			m_ownerWindow = owner_window;
			m_resources = std::unordered_map<ResourceID, ResourceCashCell>();
			m_runTimeResources = std::unordered_map<ResourceID, ResourceCashCell>();
		}

		ResourceID AddResource(std::string str_id, std::shared_ptr<IResource> resource, bool is_runtime = false)
		{
			auto id = GetNextID();
			if (is_runtime)m_runTimeResources[id] = ResourceCashCell(resource);
			else m_resources[id] = ResourceCashCell(resource);

			m_strId[str_id] = id;
			m_IdStr[id] = str_id;
			return id;
		}

		template<typename Resource>
		std::shared_ptr<Resource> GetResourceByID(ResourceID id, bool is_runtime = false) {
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			auto resource = GetResourceByID(id, is_runtime); // Базовый метод
			if (!resource) return nullptr;

			// Дополнительная проверка типа
			auto casted = std::dynamic_pointer_cast<Resource>(resource);
			if (!casted) {
				Diagnostics::Logger::Get().SendMessage("Type mismatch in GetResourceByID");
				return nullptr;
			}
			return casted;
		}

		template<typename Resource>
		std::shared_ptr<Resource> GetResourceByIDI(ResourceID id, bool is_runtime = false)	//I - Init
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			auto result = GetResourceByID<Resource>(id, is_runtime);
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
		ResourceID Load(const std::string& str_id, bool is_runtime = false) {
			auto resource = std::make_shared<Resource>();
			if (resource->Load(str_id)) {
				return AddResource(str_id, resource, is_runtime);;
			}
			return Definitions::InvalidID;
		}

		ResourceID GetIDByStrID(const std::string& str_id) 
		{
			return m_strId.count(str_id) ? m_strId.at(str_id) : Definitions::InvalidID;
		}


		bool Exists(const std::string& key, bool is_runtime = false) const;
		bool Exists(ResourceID id, bool is_runtime = false) const;

		void InitAll();	//Initialization must be after scene attachment to window
		void DeleteResource(ResourceID id, bool is_runtime = false);
		void ClearResources();

		~ResourcesFrame() 
		{
			ClearResources();
		}

		void CleanupUnusedResources();
	};
	

	class ResourceManager : public Core::IService 
	{
	private:
		Windows::WindowID m_activeWindow = Definitions::InvalidID;
	
		std::unordered_map<Windows::WindowID, ResourcesFrame> m_resources;	//Window - resources
		
	public:
		//Load и тд
		template<typename Resource>
		std::shared_ptr<Resource> Load(const std::string& str_id, bool is_runtime = false, bool init = false) 
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			
			if (m_activeWindow == Definitions::InvalidID) 
			{
				Diagnostics::Logger::Get().SendMessage("(ResourceManager) Invalid active window.");
				return nullptr;
			}

			if (!m_resources.count(m_activeWindow)) 
			{
				m_resources.insert({ m_activeWindow, ResourcesFrame(m_activeWindow) });
			}
			if (m_resources[m_activeWindow].Exists(str_id, is_runtime))
			{
				return m_resources[m_activeWindow].GetResource<Resource>(str_id,is_runtime);
			}


			std::shared_ptr<Resource> resource = nullptr;
			resource = m_resources[m_activeWindow].Load<Resource>(str_id, is_runtime);
			if (init) resource->Init();

			return resource;
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResourceByStrID(const std::string& str_id, bool is_runtime = false)
		{
			if (m_activeWindow == Definitions::InvalidID)
			{
				Diagnostics::Logger::Get().SendMessage("(ResourceManager) Invalid active window.");
				return nullptr;
			}
			if (m_resources[m_activeWindow].Exists(str_id, is_runtime))
			{
				return m_resources[m_activeWindow].GetResourceByID<Resource>(m_resources[m_activeWindow].GetIDByStrID(str_id), is_runtime);
			}
			return nullptr;
		}
		template<typename Resource>
		std::shared_ptr<Resource> GetResourceByID(ResourceID id, bool is_runtime = false)
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			if (m_activeWindow == Definitions::InvalidID)
			{
				Diagnostics::Logger::Get().SendMessage("(ResourceManager) Invalid active window.");
				return nullptr;
			}

			if (m_resources[m_activeWindow].Exists(id, is_runtime))
			{
				return m_resources[m_activeWindow].GetResourceByID<Resource>(id, is_runtime);
			}

			return nullptr;
		}

		void SetActiveWindow(Windows::WindowID new_active_window);
		void ClearWindowData(Windows::WindowID id);
		void ClearWindowCache(Windows::WindowID id);
		void Init() override;
		void Shutdown() override;
	};
}