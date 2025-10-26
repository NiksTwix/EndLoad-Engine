#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <algorithm>
#include <iostream>
#include <string> 

namespace ECS
{
	using EntityID = size_t;
	using CSP_ID = uint32_t;
	constexpr EntityID INVALID_ID = SIZE_MAX;
	constexpr EntityID ROOT_ID = SIZE_MAX;
	constexpr EntityID MAX_ENTITIES = 65536;
	constexpr EntityID MAX_COMPONENTS = 256;
	using ESpaceID = size_t;
	using PoolID = size_t;

}