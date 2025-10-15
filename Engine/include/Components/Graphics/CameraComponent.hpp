#pragma once
#include <ELECS\ECS.hpp>
#include <ELMath/include/MathFunctions.hpp>

namespace Components 
{
	enum class ProjectionType
	{
		Orthogonal,
		Perspective
	};

	struct CameraComponent 
	{
		float FOV = 90.0f;			// Only for Perspective

		float Near = 0.1f;
		float Far = 100.0f;
		float OrthoSize = 10.0f;	// Only for Orthographic
		Math::Matrix4x4 Projection;
		Math::Matrix4x4 View;
		bool isDirty = true;    // Needs matrix update
		ProjectionType ProjectionType;
		Math::Vector3 Front = Math::Vector3::Forward();
		Math::Vector3 WorldUp = Math::Vector3::Up();
		Math::Vector3 Up = WorldUp;
		Math::Vector3 Right = Math::Vector3::Right();
		Math::Vector4 FrustumPlanes[6];
		int Width;
		int Height;

	};

	class CameraComponentService: public ECS::IComponentService
	{
	
	};
}