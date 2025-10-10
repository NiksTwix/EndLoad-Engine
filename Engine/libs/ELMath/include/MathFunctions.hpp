#pragma once
#include "..\include\Vector2.hpp"
#include "..\include\Vector3.hpp"
#include "..\include\Vector4.hpp"
#include "..\include\Matrix4x4.hpp"
#include "..\include\Vertex.hpp"
#include "..\include\Quaternion.hpp"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

namespace Math
{
    constexpr float PI = 3.14159265358979323846f;

    float LengthSquared(const Vector2& v);
    float LengthSquared(const Vector3& v);
    float LengthSquared(const Vector4& v);
    float LengthSquared(const Quaternion& q);

    float Length(const Vector2& v);
    float Length(const Vector3& v);
    float Length(const Vector4& v);
    float Length(const Quaternion& q);

    Vector2 Normalize(const Vector2& v);
    Vector3 Normalize(const Vector3& v);
    Vector4 Normalize(const Vector4& v);
    Quaternion Normalize(const Quaternion& q);

    bool IsNaN(const Vector2& v);
    bool IsNaN(const Vector3& v);
    bool IsNaN(const Vector4& v);
    bool IsNaN(const Matrix4x4& m);
    bool IsNaN(const Quaternion& q);

    float EulerToRadians(float angle);
    float RadiansToEuler(float angle);

    float Dot(const Vector2& first, const Vector2& second);
    float Dot(const Vector3& first,const Vector3& second);
    float Dot(const Vector4& first,const Vector4& second);
   
    Vector3 Cross(const Vector3& first, const Vector3& second);

    const float* GetValuePtr(const Matrix4x4& m);

    Matrix4x4 Translate(const Matrix4x4& translated, const Vector4& translation);
    Matrix4x4 RotateLocal(const Vector4& axis, float radians);
    Matrix4x4 RotateLocal(const Matrix4x4& rotated, const Vector4& axis, float radians);
    Matrix4x4 Scale(const Matrix4x4& scaled, const Vector4& scale);
    Matrix4x4 Transpose(const Matrix4x4& m);
    

    Quaternion Identity();
    Quaternion Conjugate(const Quaternion& q);
    Quaternion FromEuler(float pitch, float yaw, float roll);
    Quaternion FromAxisAngle(const Vector3& axis, float angle);

    Matrix4x4 ToMatrix4x4(const Quaternion& q);

    Matrix4x4 Inverse(const Matrix4x4& m);
    Quaternion Inverse(const Quaternion& q);

    Matrix4x4 Perspective(float fovRadians, float aspect, float near, float far);
    Matrix4x4 Ortho(float left, float right, float bottom, float top, float near, float far);
    Matrix4x4 Ortho2D(float left, float right, float bottom, float top);
    Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);

    //glm::vec3 ToGLM(const Vector3& v);
    //glm::mat4 ToGLM(const Matrix4x4& m);
    //Vector3 FromGLM(const glm::vec3& v);
    //Matrix4x4 FromGLM(const glm::mat4& m);


    Vector2 Min(const Vector2& v1, const Vector2& v2);
    Vector3 Min(const Vector3& v1, const Vector3& v2);
    Vector4 Min(const Vector4& v1, const Vector4& v2);

    Vector2 Max(const Vector2& v1, const Vector2& v2);
    Vector3 Max(const Vector3& v1, const Vector3& v2);
    Vector4 Max(const Vector4& v1, const Vector4& v2);

    Vector2 Abs(const Vector2& v);
    Vector3 Abs(const Vector3& v);
    Vector4 Abs(const Vector4& v);

    std::vector<Vector3> FromStringToV3(std::string data, char delimiter = ',');
    std::vector<Vector4> FromStringToV4(std::string data, char delimiter = ',');
    std::vector<Vector2> FromStringToV2(std::string data, char delimiter = ',');
    std::vector<int> FromStringToInt(std::string data, char delimiter = ',');


}
