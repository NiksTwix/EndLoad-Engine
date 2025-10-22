#pragma once
#include <Components/Basic/TransformComponent.hpp>


namespace Components 
{
	struct MeshComponent;

	enum class ProjectionType
	{
		Orthogonal,
		Perspective
	};

	struct CameraComponent 
	{
		float fov = 90.0f;			// Only for Perspective

		float near = 0.1f;
		float far = 100.0f;
		float orthoSize = 10.0f;	// Only for Orthographic
		Math::Matrix4x4 projection;
		Math::Matrix4x4 view;
		bool isDirty = true;    // Needs matrix update
		ProjectionType projectionType;
		Math::Vector3 front = Math::Vector3::Forward();
		Math::Vector3 worldUp = Math::Vector3::Up();
		Math::Vector3 up = worldUp;
		Math::Vector3 right = Math::Vector3::Right();
		Math::Vector4 frustumPlanes[6];
		int width;
		int height;

	};

	class CameraComponentService: public ELComponentService
	{
	public:
		CameraComponentService();
		CameraComponent CreateCamera(int width, int height, ProjectionType projection_type, float fov = 90, float ortho_size = 10.0f);

		void UpdateProjectionMatrix(CameraComponent& cam, int width, int height, ProjectionType projection_type, float fov = 90);

		void UpdateViewMatrix(CameraComponent& cam, GlobalTransformComponent& transform);

		Math::Matrix4x4 GetCameraMatrix(CameraComponent& cam);

		void UpdateCameraVectors(CameraComponent& cam, GlobalTransformComponent& transform);

		void UpdateFrustumCulling(CameraComponent& cam);
		void UpdateCameraData(Scenes::SceneContext* context, ECS::EntityID entity);

		bool IsVisibleMesh(const Components::MeshComponent& mesh, const CameraComponent& camera);
	};
}