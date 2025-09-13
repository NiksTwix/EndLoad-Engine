#pragma once
#include <sstream>
#include <algorithm>
#include <ECS\ESDL\ESDLStructs.hpp>
#include <ECS\ESDL\ESDLStrings.hpp>


namespace ECS 
{
	namespace ESDL 
	{
        enum class ArrayType 
        {
            Undefined = 0,
            Number,
            String
        };


        class ValueParser {
        public:
            static ESDLType Parse(const std::string& value) {
                std::string trimmed = Strings::Trim(value);

                // Булевые значения
                if (trimmed == "true") return true;
                if (trimmed == "false") return false;

                // Векторы [x, y, z] или [x, y, z, w]
                if (auto t = GetArrayType(trimmed); t != ArrayType::Undefined) 
                {
                    switch (t)
                    {
                    case ECS::ESDL::ArrayType::Number:
                        return ParseFloatArray(trimmed);
                        break;
                    case ECS::ESDL::ArrayType::String:
                        return ParseStringArray(trimmed);
                        break;
                    default:
                        break;
                    }
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
                    else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        std::ostringstream oss;
                        oss << "[";
                        for (size_t i = 0; i < arg.size(); ++i) {
                            oss << "\"" << arg[i] << "\"";
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

            static std::vector<float> ParseFloatArray(const std::string& vecStr) {
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

            static std::vector<std::string> ParseStringArray(const std::string& vecStr) {
                std::vector<std::string> result;
                std::string inner = vecStr.substr(1, vecStr.size() - 2); // Удаляем []
                auto elements = Strings::SplitString(inner, ',');

                for (const auto& elem : elements) {
                    std::string trimmedElem = Strings::Trim(elem);
                    if (!trimmedElem.empty()) {
                        result.push_back(trimmedElem.substr(1, trimmedElem.size() - 2));
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

            static ArrayType GetArrayType(const std::string& s)     //Определяет тип массива по первому элементу, но делает проверку для всех
            {
                if (!(s.size() > 2 && s.front() == '[' && s.back() == ']')) return ArrayType::Undefined;
                
                ArrayType type = ArrayType::Undefined;

                std::string inner = s.substr(1, s.size() - 2); // Удаляем []
                auto elements = Strings::SplitString(inner, ',');

                if (elements.empty()) return ArrayType::Undefined;

                bool first_iteration = false;

                for (auto element : elements) 
                {
                    ArrayType type_ = ArrayType::Undefined;
                    if (element.size() >= 2 && element.front() == '"' && element.back() == '"')
                    {
                        type_ = ArrayType::String;
                    }
                    else if (Strings::IsNumber(element)) type_ = ArrayType::Number;

                    else return type;

                    if (!first_iteration) 
                    {
                        type = type_;
                        first_iteration = true;
                    }
                    
                    else if (type_ != type) 
                    {
                        throw std::runtime_error("ESDL error: array containes different types.");
                        return ArrayType::Undefined;
                    }
                }
                return type;
            }
        };
	}
}