#pragma once
#include <ECS\Utility\Definitions.hpp>
#include <typeindex>
#include <string>
#include <vector>
#include <memory>
#include <ECS/ESDL/ESDLStructs.hpp>

namespace ECS 
{
	class ComponentServiceLocator;
	class EntitySpace;

	/*
			IComponentService is a base class of component's services
	*/

	class IComponentService 
	{
		friend ComponentServiceLocator;
	protected:
		std::type_index m_ComponentType = typeid(void);

		std::string m_stringType = "void";

		void SetComponentType(std::type_index componentType) { m_ComponentType = componentType; }

		EntitySpace* m_eSpace = nullptr;

	public:
		IComponentService() {}
		~IComponentService() = default;

		IComponentService(const IComponentService&) = delete;
		IComponentService& operator=(const IComponentService&) = delete;

		virtual void Init() = 0;

		virtual void Update(EntitySpace* eSpace) = 0;
		virtual void Shutdown() = 0;

		std::type_index GetComponentType() { return m_ComponentType; }
		std::string GetStringComponentType() { return m_stringType; }
		//Serialization

		virtual std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) = 0;			//Return data fields of components
		
		virtual void AddComponentByFields(EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields) = 0;

		virtual void UpdateComponent(EntityID entity) {};
	};
}
