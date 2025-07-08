#pragma once
#include <vector>
#include <sstream>
#include <algorithm>
#include <ECS\ESDL\ESDLStructs.hpp>
#include <ECS\ESDL\ESDLStrings.hpp>
#include <variant>
#include <optional>


namespace ECS 
{
	namespace ESDL 
	{
        class ValueParser {
        public:
            static ESDLType Parse(const std::string& value) {
                std::string trimmed = Strings::Trim(value);

                // Булевые значения
                if (trimmed == "true") return true;
                if (trimmed == "false") return false;

                // Векторы [x, y, z] или [x, y, z, w]
                if (IsVector(trimmed)) {
                    return ParseVector(trimmed);
                }

                // Числа
                if (Strings::IsNumber(trimmed)) {
                    return ParseNumber(trimmed);
                }

                // Строки (в кавычках или без)
                return ParseString(trimmed);
            }

            static std::string ToString(const ESDLType& value)
            {
                return std::visit([](auto&& arg) -> std::string {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, int>) {
                        return std::to_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, size_t>) {
                        return std::to_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, float>) {
                        return std::to_string(arg);
                    }
                    else if constexpr (std::is_same_v<T, std::string>) {
                        return "\"" + arg + "\"";  // Заключаем строку в кавычки
                    }
                    else if constexpr (std::is_same_v<T, bool>) {
                        return arg ? "true" : "false";
                    }
                    else if constexpr (std::is_same_v<T, std::vector<float>>) {
                        std::ostringstream oss;
                        oss << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            oss << arg[i];
                            if (i != arg.size() - 1) oss << ",";
                        }
                        oss << "]";
                        return oss.str();
                    }
                    }, value);
            }
            
            template <typename T>
            static std::optional<T> GetAs(const ESDLType& value) {
                if (auto* ptr = std::get_if<T>(&value)) {
                    return *ptr;
                }
                return std::nullopt;
            }
        private:
            static bool IsVector(const std::string& s) {
                return s.size() > 2 && s.front() == '[' && s.back() == ']';
            }

            static std::vector<float> ParseVector(const std::string& vecStr) {
                std::vector<float> result;
                std::string inner = vecStr.substr(1, vecStr.size() - 2); // Удаляем []
                auto elements = Strings::SplitString(inner, ',');

                for (const auto& elem : elements) {
                    std::string trimmedElem = Strings::Trim(elem);
                    if (!trimmedElem.empty()) {
                        result.push_back(std::stof(trimmedElem));
                    }
                }

                return result;
            }

            static ESDLType ParseNumber(const std::string& numStr) {
                return numStr.find('.') != std::string::npos
                    ? std::stof(numStr)
                    : std::stoi(numStr);
            }

            static std::string ParseString(const std::string& str) {
                if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
                    return str.substr(1, str.size() - 2);
                }
                return str;
            }
        };
	}
}