#pragma once
#include <ELECS\ECS.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourcesManager.hpp>
#include <ELMath/include/MathFunctions.hpp>
#include <ELDCL/include/DCL.hpp>

namespace Components 
{
	class ELComponentService : public ECS::IComponentService //ELComponentService - adaptation of IComponentService for EL Engine 
	{
	public:
		ELComponentService() = default;

		//Work with DCL
		std::shared_ptr<DCL::Container> GetComponentFields(ECS::EntityID entity) { return nullptr; };
		void SetComponentField(ECS::EntityID entity, std::string name, DCL::Value value) {};
		void AddComponentToEntity(ECS::EntityID entity, std::shared_ptr<DCL::Container> data) {};
	};
	class IRComponentService : public ECS::IComponentService //IResourceComponentService - for work with components which contain resources 
	{
	public:
		IRComponentService() = default;
		virtual bool UpdateResourceState(ECS::EntityID entity) = 0;	//Checks and updates resource state
	};
}