#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex> 
#include <ECS\ECS.hpp>
#include <Systems\Render\Viewport.hpp>
/*
	SceneContext занимается управлением сценой и существами
*/

using SceneID = size_t;

class SceneLoader;

class SceneContext 
{
	friend SceneLoader;

	ECS::EntitySpace entitySpace;

	std::string Name = "Scene";

	SceneID sceneID = SIZE_MAX;

	std::string Description = "Description";

	std::shared_ptr<ViewportService> viewportService;

public:


	//Запрещаем копировать
	SceneContext(const SceneContext&) = delete;
	SceneContext& operator=(const SceneContext&) = delete;
	//Разрешаем перемещать
	SceneContext(SceneContext&&) = default;
	SceneContext& operator=(SceneContext&&) = default;

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
		viewportService = std::make_shared<ViewportService>(entitySpace.GetRegistryShared());
	}

	SceneContext(std::string name, SceneID id)
	{
		SetName(name);
		sceneID = id;
		viewportService = std::make_shared<ViewportService>(entitySpace.GetRegistryShared());
	}

	ViewportService& GetViewportService() 
	{
		return *viewportService;
	}

	ECS::EntitySpace& GetEntitySpace() 
	{
		return entitySpace;
	}
};