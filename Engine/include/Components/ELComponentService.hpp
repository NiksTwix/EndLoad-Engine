#pragma once
#include <ELECS\ECS.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourcesManager.hpp>
#include <ELMath/include/MathFunctions.hpp>


namespace Components 
{
	class ELComponentService : public ECS::IComponentService //ELComponentService - adaptation of IComponentService for EL Engine 
	{
	public:
		ELComponentService() = default;

		//TODO work with ELDCL
	};
	class IRComponentService : public ECS::IComponentService //IResourceComponentService - for work with components which contain resources 
	{
	public:
		IRComponentService() = default;
		virtual bool UpdateResourceState(ECS::EntityID entity) = 0;	//Checks and updates resource state
	};
}