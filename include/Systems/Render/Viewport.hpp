#pragma once
#include <unordered_map>
#include <memory>
#include <Core\IServices.hpp>
#include <Math\MathFunctions.hpp>
#include <Components\Camera.hpp>
#include <ECS\ECS.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>


class Registry;


class Viewport 
{		
private:
	int current_id;
	ECS::ComponentSmartPointer<Camera> camera;

	static int id;
	Math::Vector2 resolution;
	Math::Vector2 position;
	Math::Vector4 backcolor;
public:
	std::string Name;
	

	Viewport(int width, int height, int x = 0, int y = 0) 
	{
		
		if (width < 0) width = 100;
		if (height < 0) height = 100;

		this->resolution.x = width;
		this->resolution.y = height;

		this->position.x = x;
		this->position.y = y;

		current_id = id++;
	}

	int GetID() 
	{
		return current_id;
	}

	void SetPosition(Math::Vector2);
	Math::Vector2 GetPosition();
	void SetResolution(Math::Vector2);
	Math::Vector2 GetResolution();

	void SetCamera(ECS::Registry& reg, ECS::EntityID id);

	ECS::ComponentSmartPointer<Camera>& GetCamera();

	void SetBackgroundColor(Math::Vector4 color) { backcolor = color; }
	Math::Vector4 GetBackgroundColor() { return backcolor; }
};


class ViewportService : public IHelperService 
{
private:
	std::unordered_map<int, std::shared_ptr<Viewport>> viewports;

	std::shared_ptr<ECS::Registry> registry;

public:
	ViewportService() = default;
	ViewportService(std::shared_ptr<ECS::Registry> reg) { registry = reg; }

	std::shared_ptr<Viewport> Create( Math::Vector2 size, Math::Vector2 position = Math::Vector2(0,0), std::string name = "Viewport")
	{
		Viewport viewport(size.x,size.y,position.x,position.y);
		viewport.Name = name;
		viewports[viewport.GetID()] = std::make_shared<Viewport>(viewport);
		return viewports[viewport.GetID()];
	}

	std::shared_ptr<Viewport> Get(int id) 
	{
		if (viewports.count(id) <= 0) return nullptr;
		return viewports[id]; 
	}

	void Add(std::shared_ptr<Viewport> viewport) //Если айди занято, то вьюпорт заменяется
	{
		viewports[viewport->GetID()] = viewport;
	}

	std::vector<Viewport*> GetViewports() 
	{
		std::vector<Viewport*> result;

		for (auto& k : viewports) 
		{
			result.push_back(k.second.get());
		}
		return result;
	}

	void Delete(int id) //Удаляет вьюпорт из реестра
	{
		if (viewports.count(id) <= 0) return;
		viewports.erase(id);
	}
	void Shutdown() override;

	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetViewportFields(int id);
	std::shared_ptr<Viewport> CreateViewportByFields(std::unordered_map<std::string, ECS::ESDL::ESDLType> fields);
};