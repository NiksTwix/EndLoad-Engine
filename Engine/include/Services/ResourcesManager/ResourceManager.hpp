#pragma once
#include <Core\IServices.hpp>
#include <Systems\Graphics\Windows\Window.hpp>
#include <Resources\IResource.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>

namespace Resources
{
	struct ResourceFrame {
	private:
		std::unordered_map<std::string, std::shared_ptr<IResource>> m_resources;
		std::unordered_map<std::string, std::shared_ptr<IResource>> m_runTimeResources;	//Name-resource
		Windows::WindowID m_ownerWindow = Definitions::InvalidID;

	public:

		ResourceFrame() = default;

		ResourceFrame(Windows::WindowID owner_window) 
		{
			m_ownerWindow = owner_window;
			m_resources = std::unordered_map<std::string, std::shared_ptr<IResource>>();
			m_runTimeResources = std::unordered_map<std::string, std::shared_ptr<IResource>>();
		}

		void AddResource(std::string path, std::shared_ptr<IResource> resource, bool is_runtime = false) 
		{
			if (is_runtime)m_runTimeResources[path] = resource;
			else m_resources[path] = resource;
		}


		template<typename Resource>
		Resource* GetResourceRaw(std::string path, bool is_runtime = false)
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");

			if (is_runtime) 
			{
				if (!m_runTimeResources.count(path)) return nullptr;
				return dynamic_cast<Resource*>(m_runTimeResources.at(path).get());
			}
			if (!m_resources.count(path)) return nullptr;
			return dynamic_cast<Resource*>(m_resources.at(path).get());
		}

		template<typename Resource>
		std::shared_ptr<Resource> GetResource(std::string path, bool is_runtime = false)
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			if (is_runtime)
			{
				if (!m_runTimeResources.count(path)) return nullptr;
				return std::dynamic_pointer_cast<Resource>(m_runTimeResources.at(path));
			}
			if (!m_resources.count(path)) return nullptr;
			return std::dynamic_pointer_cast<Resource>(m_resources.at(path));
		}

		void DeleteResource(std::string path, bool is_runtime = false)
		{
			if (is_runtime) 
			{
				if (!m_runTimeResources.count(path)) return;
				m_runTimeResources.erase(path);
			}
			else 
			{
				if (!m_resources.count(path)) return;
				m_resources.erase(path);
			}
			
		}
		void ClearResources() 
		{
			for (auto& [path, resource] : m_resources) 
			{
				resource->Release();
				Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Resource \"" + path + "\" has released. Window id: " + std::to_string(m_ownerWindow) + ".");
			}
			for (auto& [name, resource] : m_runTimeResources)
			{
				resource->Release();
				Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Runtime resource \"" + name + "\" has released. Window id: " + std::to_string(m_ownerWindow) + ".");
			}
			m_resources.clear();
			m_runTimeResources.clear();
		}

		template<typename Resource>
		std::shared_ptr<Resource> Load(const std::string& path, bool is_runtime = false) {
			auto resource = std::make_shared<Resource>();
			if (resource->Load(path)) {
				if (!is_runtime)m_resources[path] = resource;
				else m_runTimeResources[path] = resource;
				return resource;
			}
			return nullptr;
		}

		template<typename Resource>
		std::shared_ptr<Resource> LoadAndInit(const std::string& path, bool is_runtime = false) {
			auto resource = std::make_shared<Resource>();
			if (resource->Load(path) && resource->Init()) {
				if (!is_runtime)m_resources[path] = resource;
				else m_runTimeResources[path] = resource;
				return resource;
			}
			return nullptr;
		}

		bool Exists(const std::string& key, bool is_runtime = false)
		{
			if (is_runtime) return m_runTimeResources.count(key);
			return m_resources.count(key);
		}

		void InitAll()	//Initialization must be after scene attachment to window
		{
			for (auto& [path, resource] : m_resources)
			{
				if (resource->GetState() == ResourceState::Loaded)resource->Init();
				if (resource->GetState() == ResourceState::Initialized)Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Resource \"" + path + "\" has initialized. Window id: " + std::to_string(m_ownerWindow) + ".");
			}
			for (auto& [name, resource] : m_runTimeResources)
			{
				if (resource->GetState() == ResourceState::Loaded)resource->Init();
				if (resource->GetState() == ResourceState::Initialized)Diagnostics::Logger::Get().SendMessage("(ResourceFrame) Runtime resource \"" + name + "\" has initialized. Window id: " + std::to_string(m_ownerWindow) + ".");
			}
		}

		~ResourceFrame() 
		{
			ClearResources();
		}
	};
	

	class ResourceManager : public Core::IService 
	{
	private:
		Windows::WindowID m_activeWindow = Definitions::InvalidID;
	
		std::unordered_map<Windows::WindowID, ResourceFrame> m_resources;	//Window - resources
		
	public:

		//Load ט עה

		template<typename Resource>
		std::shared_ptr<Resource> Load(const std::string& path, bool is_runtime = false, bool init = false) 
		{
			static_assert(std::is_base_of_v<IResource, Resource>, "Resource must inherit from IResource");
			
			if (m_activeWindow == Definitions::InvalidID) 
			{
				Diagnostics::Logger::Get().SendMessage("(ResourceManager) Invalid active window.");
				return nullptr;
			}

			if (!m_resources.count(m_activeWindow)) 
			{
				m_resources.insert({ m_activeWindow, ResourceFrame(m_activeWindow) });
			}
			if (m_resources[m_activeWindow].Exists(path, is_runtime))
			{
				return m_resources[m_activeWindow].GetResource<Resource>(path,is_runtime);
			}


			std::shared_ptr<Resource> resource = nullptr;
			if (!init) resource = m_resources[m_activeWindow].Load<Resource>(path, is_runtime);
			else resource = m_resources[m_activeWindow].LoadAndInit<Resource>(path, is_runtime);

			return resource;
		}

		void SetActiveWindow(Windows::WindowID new_active_window) 
		{
			m_activeWindow = new_active_window;
		}

		void ClearWindowData(Windows::WindowID id)
		{
			if (m_resources.count(id))
			{
				m_resources[id].ClearResources();
				m_resources.erase(id);
			}
		}

		void Init() override;
		void Shutdown() override;

	};
}