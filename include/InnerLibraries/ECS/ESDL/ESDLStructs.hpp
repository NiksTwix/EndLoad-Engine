#pragma once
#include <ECS\Utility\Definitions.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>



namespace ECS 
{
	namespace ESDL 
	{
		using ESDLType = std::variant<int, float, std::string, bool, std::vector<float>, EntityID>;

		struct ComponentTemplate {
			std::string type;
			std::unordered_map<std::string, ESDLType> properties;
		};
		struct EntityTemplate 
		{
			EntityID id;
			EntityID parentID = INVALID_ID;
			std::vector<ComponentTemplate> components;
		};

		struct DataBlockTemplate 
		{
			std::unordered_map<std::string, ESDLType> properties;
		};
	}

}