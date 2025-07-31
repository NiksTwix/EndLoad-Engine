#pragma once
#include <Math\Vector3.hpp>
#include <ECS\Utility\Definitions.hpp>

struct AABB
{
public:
	Math::Vector3 Max;
	Math::Vector3 Min;
};