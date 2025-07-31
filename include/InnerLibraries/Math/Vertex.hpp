#pragma once
#include <Math\Vector2.hpp>
#include <Math\Vector3.hpp>


namespace Math 
{
	struct Vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texCoord;
	};
}
