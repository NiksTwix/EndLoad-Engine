#pragma once
#include <iostream>
#include <limits>

namespace ECS
{
	using EntityID = size_t;
	using CSP_ID = uint32_t;
	constexpr EntityID INVALID_ID = std::numeric_limits<EntityID>::max();
	constexpr EntityID ROOT_ID = std::numeric_limits<EntityID>::max();
	constexpr EntityID MAX_ENTITIES = 65536;

	using PoolID = size_t;

}