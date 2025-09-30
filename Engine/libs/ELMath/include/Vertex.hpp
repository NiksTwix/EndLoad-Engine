#pragma once
#include "Vector2.hpp"
#include "Vector3.hpp"


namespace Math 
{
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texCoord;
		Vector3 tangent;
		Vector3 bitangent;
	};
}
