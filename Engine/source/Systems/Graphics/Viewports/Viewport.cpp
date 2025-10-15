#include <Systems/Graphics/Viewports/Viewport.hpp>
#include <Core\ServiceLocator.hpp>


namespace Viewports 
{
	Definitions::identificator Viewport::id = 0;


	void ViewportService::Shutdown()
	{
		viewports.clear();
	}

	std::unordered_map<std::string, ECS::ESDL::ESDLType> ViewportService::GetViewportFields(int id)
	{
		std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
		if (viewports.count(id) == 0) return result;

		auto viewport = viewports[id];

		auto resolution = viewport->GetResolution();
		auto position = viewport->GetPosition();
		auto id_ = viewport->GetID();
		auto bgcolor = viewport->GetBackgroundColor();
		auto camera = (int)viewport->GetCamera().GetEntityID();	//Ограничение в 2 млрд сущностей

		result["ID"] = id_;
		result["Name"] = viewport->Name;
		result["Resolution"] = resolution.ToArray();
		result["Position"] = position.ToArray();
		result["BackgroundColor"] = bgcolor.ToArray();
		if (camera == ECS::INVALID_ID) result["Camera"] = "NONE";
		else result["Camera"] = camera;

		return result;
	}

	std::shared_ptr<Viewport> ViewportService::CreateViewportByFields(std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
	{
		int id = -1;
		std::string name = "Viewport";
		Math::Vector2 resolution(100, 100);
		Math::Vector2 position;
		Math::Vector4 bgColor;
		int camera = -1;

		for (auto& [field, value] : fields)
		{
			if (field == "Camera")
			{
				bool result = ECS::ESDL::TryConvertField<int>(camera, value);
			}
			if (field == "ID")
			{
				ECS::ESDL::TryConvertField<int>(id, value);
			}
			if (field == "Name")
			{
				if (auto wu = ECS::ESDL::GetAs<std::string>(value)) name = *wu;
			}
			if (field == "Resolution")
			{
				if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(value)) {
					resolution = Math::Vector2::FromArray(*wu);
				}
			}
			if (field == "Position")
			{
				if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(value)) {
					position = Math::Vector2::FromArray(*wu);
				}
			}
			if (field == "BackgroundColor")
			{
				if (auto wu = ECS::ESDL::GetAs<std::vector<float>>(value)) {
					bgColor = Math::Vector4::FromArray(*wu);
				}
			}
		}

		auto viewport = Create(resolution, position, name);

		viewport->SetBackgroundColor(bgColor);
		if (registry && camera != -1) viewport->SetCamera(*registry, camera);
		viewport->SetBackgroundColor(bgColor);
		return viewport;
	}

	void Viewport::SetPosition(Math::Vector2 new_position)
	{
		position = new_position;
	}

	Math::Vector2 Viewport::GetPosition()
	{
		return position;
	}

	void Viewport::SetResolution(Math::Vector2 new_resolution)
	{
		resolution = new_resolution;
		matrix = Math::Ortho2D(0, resolution.x, 0, resolution.y);
	}

	void Viewport::Scale(Math::Vector2 scale)
	{
		std::cout << this->scale.ToString() << "\n";
		position *= scale;
		resolution *= scale;
		this->scale *= scale;
	}

	Math::Vector2 Viewport::GetResolution()
	{
		return resolution;
	}

	Math::Matrix4x4 Viewport::GetMatrix()
	{
		return matrix;
	}

	void Viewport::SetCamera(ECS::Registry& reg, ECS::EntityID id)
	{
		if (id == ECS::INVALID_ID) return;
		camera = ECS::ComponentSmartPointer<Components::CameraComponent>(reg, id);
	}

	ECS::ComponentSmartPointer<Components::CameraComponent>& Viewport::GetCamera()
	{
		return camera;
	}
}


