#pragma once
#include <unordered_map>
#include <vector>
#include <assert.h>
#include <memory>
#include <limits.h>

//This file contains constant and other values

//Type templates

namespace Definitions {

	using uint = unsigned int;

	using identificator = size_t;

	//Constants
	constexpr float EngineVersion = 0.2f;
	constexpr identificator InvalidID = SIZE_MAX;
}
