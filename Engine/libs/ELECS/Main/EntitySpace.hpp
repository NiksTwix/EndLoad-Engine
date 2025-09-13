#pragma once
#include <ECS\Registry\Registry.hpp>
#include <ECS\Tree\HierarchyTree.hpp>
#include <ECS/ComponentServices/ComponentServiceLocator.hpp>
#include <ECS\Events\EventsManager.hpp>
#include <vector>
#include <memory>

namespace ECS 
{
	

	class EntitySpace 
	{
	private:
		std::shared_ptr<Registry> registry;
		std::shared_ptr<HierarchyTree> tree;
		std::vector<ECS::EntityID> freeList;

		std::shared_ptr<ComponentServiceLocator> serviceLocator;
		std::shared_ptr<EventsManager> eventsManager;
		EntityID nextID = 1;

		ESpaceID es_id = INVALID_ID;

		static ESpaceID GetNextID()
		{
			static ESpaceID next_id = 0;
			return next_id++;
		}

	public:

		HierarchyTree& GetTree()
		{
			return *tree;
		}
		Registry& GetRegistry()  
		{
			return *registry;
		}
		EventsManager& GetEventsManager()
		{
			return *eventsManager;
		}

		CSL& GetServiceLocator()						//Returns ComponentServiceLocator
		{
			return *serviceLocator;
		}

		std::shared_ptr<HierarchyTree> GetTreeShared()
		{
			return tree;
		}
		std::shared_ptr<Registry> GetRegistryShared()
		{
			return registry;
		}

		EntitySpace() 
		{
			es_id = GetNextID();
			registry = std::make_shared<Registry>();
			tree = std::make_shared<HierarchyTree>();
			serviceLocator = std::make_shared<ComponentServiceLocator>();
			eventsManager = std::make_shared<EventsManager>();
		}

		ESpaceID GetID() 
		{
			return es_id;
		}

		//Entities

		EntityID CreateEntity()			//Creates entity with free id
		{
			EntityID id = INVALID_ID;
			if (!freeList.empty())
			{
				id = freeList.back();
				freeList.pop_back();
			}
			else id = nextID++;

			return id;
		}

		void AddEntity(EntityID id)				//Adds entity to tree (to root node)
		{
			tree->AddEntityToRoot(id);
		}

		EntityID CreateEntityWithID(ECS::EntityID id)
		{
			if (id == ECS::INVALID_ID) return  ECS::INVALID_ID;
			if (tree->HasEntity(id))
			{
				return  ECS::INVALID_ID;
			}
			AddEntity(id);
			if (nextID <= id) nextID = id + 1;	//WARNING может содержать ошибки
			return id;
		}

		void AddEntity(EntityID id, EntityID parent)				//Adds entity to tree (to parent)
		{
			tree->AddEntityToEntity(parent, id);
		}

		void RemoveEntity(EntityID id)				//Removes entity from tree and delete all it's components
		{
			if (!tree->HasEntity(id))
				return;

			auto children = tree->GetChildren(id);

			for (auto id : children) 
			{
				RemoveEntity(id);
			}


			tree->RemoveEntity(id);
			registry->RemoveAll(id);
			freeList.push_back(id);
		}

		bool IsEntityExists(EntityID id) const
		{
			return tree->HasEntity(id);
		}

		void ClearAllEntities() 
		{
			auto entities = GetAllEntities();

			for (auto entity : entities) 
			{
				RemoveEntity(entity);
			}
			nextID = 1;
		}


		std::vector<EntityID> GetAllEntities()
		{
			return tree->GetAllEntities();
		}

		std::vector<EntityID>& GetFreeEntities() 
		{
			return freeList;
		}

		//Components

		template<typename T>
		void AddComponent(EntityID id, const T& component)		//Add component to entity, component will be copied
		{
			registry->Add<T>(id, component);
		}

		template<typename T>
		T& GetComponent(EntityID id)					//Return component, if components doesnt exist return nullptr
		{
			return registry->Get<T>(id);
		}

		template<typename T>
		void RemoveComponent(EntityID id)					//Removes components
		{
			registry->Remove<T>(id);
		}
		template<typename T>
		bool HasComponent(EntityID id) const
		{
			return registry->Has<T>(id);
		}

		

		void UpdateAllServices() 
		{
			for (auto service : serviceLocator->GetAllServices()) 
			{
				service->Update(this);
			}
		}
	};

}
