#pragma once
#include <ECS\ECS.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>
#include <Math\MathFunctions.hpp>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::string> SplitString(const std::string& str, char delimiter, int limit = -1);


bool StartsWith(const std::string& source, const std::string& start);
bool EndsWith(const std::string& source, const std::string& end);

std::vector<std::string> SplitToLines(const std::string& str);

std::string EraseString(const std::string& str, const std::string& sub_str);


bool IsEqualAround(float first, float second, float precision);



template <typename T>
bool TrySetNumericField(T& field, const ECS::ESDL::ESDLType& value) {
    if (auto val = ECS::ESDL::ValueParser::GetAs<float>(value)) {
        field = static_cast<T>(*val);
        return true;
    }
    if (auto val = ECS::ESDL::ValueParser::GetAs<size_t>(value)) {
        field = static_cast<T>(*val);
        return true;
    }
    if (auto val = ECS::ESDL::ValueParser::GetAs<int>(value)) {
        field = static_cast<T>(*val);
        return true;
    }
    return false;
}

inline bool TrySet3DVector(Math::Vector3& vector3d, const ECS::ESDL::ESDLType& value) {
    if (auto vec = ECS::ESDL::ValueParser::GetAs<std::vector<float>>(value)) {
        if (vec->size() >= 3) {
            vector3d.x = (*vec)[0];
            vector3d.y = (*vec)[1];
            vector3d.z = (*vec)[2];
            return true;
        }
    }
    return false;
}
