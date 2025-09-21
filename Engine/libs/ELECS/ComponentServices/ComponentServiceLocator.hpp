#pragma once
#include "IComponentService.hpp"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace ECS 
{
	class ComponentServiceLocator 
	{
	private:
		std::unordered_map<std::type_index, std::shared_ptr<IComponentService>> m_services;					//Key - component type
		std::unordered_map<std::type_index, std::shared_ptr<IComponentService>> m_servicesTyped;			//Key - service type
		std::unordered_map<std::string, std::shared_ptr<IComponentService>> m_servicesStringTyped;				//Key - service type in string format
		mutable std::mutex m_mutex;

	public:
		ComponentServiceLocator() = default;
		ComponentServiceLocator(const ComponentServiceLocator&) = delete;
		ComponentServiceLocator& operator=(const ComponentServiceLocator&) = delete;


		template<typename ComponentTemplate, typename Service>
		Service* Register()
		{
			static_assert(std::is_base_of_v<IComponentService, Service>,
				"Service must inherit from IComponentService");
			static_assert(std::is_default_constructible_v<Service>,
				"Service must be default constructible");

			std::lock_guard<std::mutex> lock(m_mutex);

			// �������� type_index ��� ���������� � �������
			const auto ctype = std::type_index(typeid(ComponentTemplate));
			const auto stype = std::type_index(typeid(Service));

			// ���������, �� ��������������� �� ��� ������ ��� ����� ����������
			if (m_services.find(ctype) != m_services.end()) {
				return static_cast<Service*>(m_services.at(ctype).get());
			}

			// ������� � �������������� ������
			auto service = std::make_shared<Service>();
			service->Init();

			// ������������� ��� ���������� ����� ���������� �����
			service->SetComponentType(ctype);

			// ������������ ������
			m_services[ctype] = service;
			m_servicesTyped[stype] = service;
			m_servicesStringTyped[m_services[ctype]->GetStringComponentType()] = service;
			return service.get();
		}

		template<typename ComponentTemplate>
		IComponentService* GetByComponent() 
		{
			auto ctype = std::type_index(typeid(ComponentTemplate));

			if (auto it = m_services.find(ctype); it != m_services.end()) {
				return m_services.at(ctype).get();
			}
			return nullptr;
		}

		template<typename Service>
		Service* GetByService()
		{
			static_assert(std::is_base_of_v<IComponentService, Service>,
				"Service must inherit from IComponentService");

			auto stype = std::type_index(typeid(Service));
			if (auto it = m_servicesTyped.find(stype); it != m_servicesTyped.end()) {
				return static_cast<Service*>(m_servicesTyped.at(stype).get());
			}
			return nullptr;
		}

		IComponentService* GetByTypeIndex(std::type_index type) 
		{
			IComponentService* ptr = nullptr;
			if (m_services.count(type) != 0) ptr = m_services[type].get();
			else if (m_servicesTyped.count(type) != 0)
			{
				ptr = m_servicesTyped[type].get();
			}
			return ptr;
		}
		IComponentService* GetByTypeString(const std::string& type)
		{
			IComponentService* ptr = nullptr;
			if (m_servicesStringTyped.count(type) != 0) ptr = m_servicesStringTyped[type].get();
			return ptr;
		}

		template<typename ComponentTemplate>
		void Remove() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_services.count(std::type_index(typeid(ComponentTemplate))) == 0) return;
			m_servicesTyped.erase(m_services[std::type_index(typeid(ComponentTemplate))]);
			m_services.erase(std::type_index(typeid(ComponentTemplate)));
			
		}

		std::unordered_map<std::type_index,IComponentService*> GetAllServicesTyped() 
		{
			std::unordered_map<std::type_index, IComponentService*> result;
			for (auto& ty : m_services) 
			{
				result.insert({ std::type_index(ty.first),ty.second.get() });
			}
			return result;
		}
		std::vector<IComponentService*> GetAllServices()
		{
			std::vector<IComponentService*> result;
			for (auto& ty : m_services)
			{
				result.push_back(ty.second.get());
			}
			return result;
		}

		~ComponentServiceLocator() {
			std::lock_guard<std::mutex> lock(m_mutex);
			// 1. ������� ������������� ��� �������
			for (auto& [type, service] : m_services) {
				if (service) {
					service->Shutdown(); // ������ ���� ����� � IComponentService
				}
			}

			// 2. ����� �������
			m_servicesTyped.clear(); // ������� ��������� ���������
			m_services.clear();      // ����� ��������
		}
	};


	using CSL = ComponentServiceLocator;
}