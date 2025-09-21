#pragma once
#include "..\Utility\Definitions.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <variant>



namespace ECS 
{
	namespace ESDL 
	{
		using ESDLType = std::variant<int, float, std::string, bool, std::vector<float>, std::vector<std::string>>;

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

		template <typename T>
		static std::optional<T> GetAs(const ESDLType& value) {
			if (auto* ptr = std::get_if<T>(&value)) {
				return *ptr;
			}
			return std::nullopt;
		}

		template<typename T>
		bool TryConvertField(T& target, const ESDLType& value) {
			std::optional<T> val = GetAs<T>(value);
			if (val.has_value()) {
				target = val.value();
				return true;
			}
			return false;
		}

		template<>
		inline bool TryConvertField<int>(int& target, const ESDLType& value) {
			// —начала пробуем получить как int
			if (auto val = GetAs<int>(value)) {
				target = *val;
				return true;
			}

			// ≈сли не получилось, пробуем как float и конвертируем
			if (auto val_float = GetAs<float>(value)) {
				target = static_cast<int>(*val_float);  // явное приведение
				return true;
			}

			return false;
		}
	}

}