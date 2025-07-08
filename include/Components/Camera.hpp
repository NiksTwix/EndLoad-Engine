#pragma once
#include <string>
#include <Math\MathFunctions.hpp>
#include <memory>
#include <Core/IServices.hpp>
#include <Components\Transform.hpp>
#include <Components\AABB.hpp>
#include <vector>
#include <ECS\ECS.hpp>

//class CameraService;

enum class ProjectionType 
{
	Orthogonal,
	Perspective
};


struct Camera 
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

	Transform GlobalTransform;//Глобальная трансформация

	int Width;
	int Height;

private:
	bool IsCurrent;

	
};


class CameraService : public ECS::IComponentService 
{
private:
	Camera CurrentCamera;

	//TODO ПЕРЕМЕСТИТЬ В НАСТРОЙКИ

	bool enableFrustrumCulling = false;

	Camera* RenderCamera;

public:

	CameraService();

	Camera CreateCamera(int width, int height, ProjectionType projection_type,float fov = 90, float ortho_size = 10.0f);



	void UpdateProjectionMatrix(Camera& cam, int width, int height, ProjectionType projection_type, float fov = 90);

	void UpdateViewMatrix(Camera& cam, Transform& transform);

	Math::Matrix4x4 GetCameraMatrix(Camera& cam);

	void UpdateCameraVectors(Camera& cam, Transform& transform);

	void UpdateFrustumCulling(Camera& cam);

	void UpdateAllCameras(SceneContext& context);

	bool IsCurrentValid() { return CurrentCamera.IsCurrent; }

	Camera& GetCurrent();

	Camera* GetRenderCamera() { return RenderCamera; };

	void SetRenderCamera(Camera* cam) { RenderCamera = cam; };

	void SetAsCurrent(Camera& cam);

	bool IsVisibleAABB(const AABB& aabb,const Camera& camera);



	// Унаследовано через IComponentService

	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;

	void Init() override;
	void Shutdown() override;

	// Унаследовано через IComponentService
	void Update(ECS::EntitySpace* eSpace) override;
};
