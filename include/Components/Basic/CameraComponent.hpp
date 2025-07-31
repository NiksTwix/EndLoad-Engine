#pragma once
#include <string>
#include <Math\MathFunctions.hpp>
#include <memory>
#include <Core/IServices.hpp>
#include <Components\Basic\TransformComponent.hpp>
#include <Components\Basic\AABB.hpp>
#include <vector>
#include <ECS\ECS.hpp>

//class CameraService;

enum class ProjectionType 
{
	Orthogonal,
	Perspective
};


struct CameraComponent 
{
	friend class CameraService;


	float FOV = 90.0f;			// Only for Perspective

	float Near = 0.1f;
	float Far = 100.0f; 
	float OrthoSize = 10.0f;	// Only for Orthographic

	Math::Matrix4x4 Projection;
	Math::Matrix4x4 View;
	bool isDirty = true;    // Needs matrix update
	
	ProjectionType Projection_Type;

	Math::Vector3 Front;//Z вектор
	Math::Vector3 Up;//Y вектор
	Math::Vector3 WorldUp = Math::Vector3(0,1,0);//Y вектор в мировых координатах
	Math::Vector3 Right;//X вектор

	Math::Vector4 FrustumPlanes[6];

	int Width;
	int Height;

private:
	bool IsCurrent;
	
};


class CameraService : public ECS::IComponentService 
{
private:
	CameraComponent CurrentCamera;

	//TODO ПЕРЕМЕСТИТЬ В НАСТРОЙКИ

	bool enableFrustrumCulling = false;

	ECS::ComponentSmartPointer<CameraComponent> RenderCamera;

public:

	CameraService();

	CameraComponent CreateCamera(int width, int height, ProjectionType projection_type,float fov = 90, float ortho_size = 10.0f);



	void UpdateProjectionMatrix(CameraComponent& cam, int width, int height, ProjectionType projection_type, float fov = 90);

	void UpdateViewMatrix(CameraComponent& cam, GlobalTransformComponent& transform);

	Math::Matrix4x4 GetCameraMatrix(CameraComponent& cam);

	void UpdateCameraVectors(CameraComponent& cam, GlobalTransformComponent& transform);

	void UpdateFrustumCulling(CameraComponent& cam);

	void UpdateAllCameras(SceneContext& context);

	bool IsCurrentValid() { return CurrentCamera.IsCurrent; }

	CameraComponent& GetCurrent();

	CameraComponent* GetRenderCamera() { return RenderCamera.Get(); };
	ECS::EntityID GetRenderCameraID() { return RenderCamera.GetEntityID(); };

	bool IsRenderCamera(ECS::EntityID cam) { return RenderCamera.GetEntityID() == cam; }

	void SetRenderCamera(ECS::EntityID camera) 
	{ 
		RenderCamera.Rebuild(m_eSpace->GetRegistry(), camera);
	};

	void SetAsCurrent(CameraComponent& cam);

	bool IsVisibleAABB(const AABB& aabb,const CameraComponent& camera);



	// Унаследовано через IComponentService

	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;

	void Init() override;
	void Shutdown() override;

	// Унаследовано через IComponentService
	void Update(ECS::EntitySpace* eSpace) override;
};
