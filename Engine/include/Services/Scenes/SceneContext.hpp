#pragma once
#include <memory>
#include <typeindex> 
#include <ELECS\ECS.hpp>
#include <Systems\Graphics\Viewports\Viewport.hpp>
#include <Resources\IResource.hpp>
/*
	SceneContext controls scene and entities
*/

namespace Scenes 
{
	class SceneLoader;

	class SceneContext
	{
		friend SceneLoader;

		ECS::EntitySpace entitySpace;

		std::string Name = "Scene";

		SceneID sceneID = Definitions::InvalidID;

		std::string Description = "Description";

		std::shared_ptr<Viewports::ViewportService> viewportService;

		std::vector<std::shared_ptr<Resources::IResource>> staticResources;

		void ClearStaticResources()
		{
			for (auto r : staticResources)
			{
				r->Release();	//Uninit() call only in ResourceFrame
			}
			staticResources.clear();
		}
	public:
		//Copying is forbidden
		SceneContext(const SceneContext&) = delete;
		SceneContext& operator=(const SceneContext&) = delete;
		//Разрешаем перемещать
		SceneContext(SceneContext&&) = default;
		SceneContext& operator=(SceneContext&&) = default;

		std::vector<std::shared_ptr<Resources::IResource>>  GetStaticResources() const
		{
			return staticResources;
		}
		std::string GetName() { return Name; }
		void SetName(std::string new_name)
		{
			Name = new_name;
		}

		std::string GetDescription() { return Description; }

		void SetDescription(std::string new_description)
		{
			Description = new_description;
		}

		SceneID GetID()
		{
			return sceneID;
		}


		SceneContext(std::string name)
		{
			SetName(name);
			viewportService = std::make_shared<Viewports::ViewportService>(entitySpace.GetRegistryShared());
		}

		SceneContext(std::string name, SceneID id)
		{
			SetName(name);
			sceneID = id;
			viewportService = std::make_shared<Viewports::ViewportService>(entitySpace.GetRegistryShared());
		}
		SceneContext(std::string name, SceneID id, std::vector<std::shared_ptr<Resources::IResource>> resources)
		{
			SetName(name);
			sceneID = id;
			viewportService = std::make_shared<Viewports::ViewportService>(entitySpace.GetRegistryShared());
			staticResources = resources;
		}
		Viewports::ViewportService& GetViewportService()
		{
			return *viewportService;
		}

		ECS::EntitySpace& GetEntitySpace()
		{
			return entitySpace;
		}
		~SceneContext() 
		{
			ClearStaticResources();
		}
	};
}
