#pragma once
#include <ELECS\ECS.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>
#include <ELMath/include/MathFunctions.hpp>


namespace Components 
{
	class IRComponentService : public ECS::IComponentService //IResourceComponentService - for work with components which contain resources 
	{
	public:
		virtual bool UpdateResourceState(ECS::EntityID entity) = 0;	//Checks and updates resource state
	};
}