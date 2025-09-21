#pragma once
#include "..\Utility\Definitions.hpp"
#include <vector>
#include <typeindex>
namespace ECS 
{
	class ComponentPool
	{
	public:
		~ComponentPool() = default;

		virtual bool Has(EntityID entity) const = 0;
		virtual bool Add(EntityID entity, const void* component) = 0;
		virtual bool Remove(EntityID entity) = 0;
		virtual void* Get(EntityID entity) = 0;
		virtual const void* Get(EntityID entity) const = 0;
		virtual std::type_index GetComponentType() const = 0;

		virtual std::vector<EntityID> GetAllEntities() = 0;

		virtual void Update() = 0;
	};


}
