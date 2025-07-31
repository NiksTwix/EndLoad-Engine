#include <Math\MathFunctions.hpp>
#include <cmath>



namespace Math
{
    float LengthSquared(const Vector2& v) { return v.x * v.x + v.y * v.y; }
    float LengthSquared(const Vector3& v) { return v.x * v.x + v.y * v.y + v.z * v.z; }
    float LengthSquared(const Vector4& v) { return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w; }
    float LengthSquared(const Quaternion& q) { return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w; }

    float Length(const Vector2& v) { return std::sqrt(LengthSquared(v)); }
    float Length(const Vector3& v) { return std::sqrt(LengthSquared(v)); }
    float Length(const Vector4& v) { return std::sqrt(LengthSquared(v)); }
    float Length(const Quaternion& q) { return std::sqrt(LengthSquared(q)); }

    Vector2 Normalize(const Vector2& v) {
        float len = Length(v);
        return len > 0.0f ? Vector2(v.x / len, v.y / len) : Vector2();
    }

    Vector3 Normalize(const Vector3& v) {
        float len = Length(v);
        return len > 0.0f ? Vector3(v.x / len, v.y / len, v.z / len) : Vector3();
    }

    Vector4 Normalize(const Vector4& v) {
        float len = Length(v);
        return len > 0.0f ? Vector4(v.x / len, v.y / len, v.z / len, v.w / len) : Vector4();
    }

    Quaternion Normalize(const Quaternion& q)
    {
        float len = Length(q);
        return len > 0.0f ? Quaternion(q.x / len, q.y / len, q.z / len, q.w / len) : Quaternion();
    }

    bool IsNaN(const Vector2& v) { return std::isnan(v.x) && std::isnan(v.y); }
    bool IsNaN(const Vector3& v) { return std::isnan(v.x) && std::isnan(v.y) && std::isnan(v.z); }
    bool IsNaN(const Vector4& v) { return std::isnan(v.x) && std::isnan(v.y) && std::isnan(v.z) && std::isnan(v.w); }
    bool IsNaN(const Matrix4x4& m)
    {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (std::isnan(m.values[i][j])) {
                    return true;
                }
            }
        }
        return false;
    }
    bool IsNaN(const Quaternion& q) { return std::isnan(q.x) && std::isnan(q.y) && std::isnan(q.z) && std::isnan(q.w); }

    float EulerToRadians(float angle) { return angle * PI / 180; }
    float RadiansToEuler(float angle) { return angle * 180 / PI; }

    float Dot(const Vector2& first, const Vector2& second) 
    {
        return first.x * second.x + first.y * second.y;
    }
    float Dot(const Vector3& first, const Vector3& second) 
    {
       return first.x * second.x + first.y * second.y + first.z * second.z;
    }
    float Dot(const Vector4& first, const Vector4& second)
    {
        return first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;
    }
    Vector3 Cross(const Vector3& first, const Vector3& second)
    {
        return Vector3(
            first.y * second.z - first.z * second.y,
            first.z * second.x - first.x * second.z,
            first.x * second.y - first.y * second.x
        );
    }

    const float* GetValuePtr(const Matrix4x4& m) { return &(m.values[0][0]); }


    Matrix4x4 Translate(const Matrix4x4& translated, const Vector4& translation){
        return Matrix4x4
        (
            translated.x0, translated.x1, translated.x2, translated.x3 + translation.x,
            translated.y0, translated.y1, translated.y2, translated.y3 + translation.y,
            translated.z0, translated.z1, translated.z2, translated.z3 + translation.z,
            translated.w0, translated.w1, translated.w2, translated.w3 + translation.w
        );
    }

    Matrix4x4 RotateLocal(const Vector4& axis, float radians) 
    {
        float c = std::cos(radians);
        float s = std::sin(radians);

        
        Vector3 normAxis = Normalize(axis);
        float x = normAxis.x;
        float y = normAxis.y;
        float z = normAxis.z;
        float t = 1.0f - c;

        return Matrix4x4(
            t * x * x + c, t * x * y - s * z, t * x * z + s * y, 0,
            t * x * y + s * z, t * y * y + c, t * y * z - s * x, 0,
            t * x * z - s * y, t * y * z + s * x, t * z * z + c, 0,
            0, 0, 0, 1
        );

    }

    Matrix4x4 RotateLocal(const Matrix4x4& rotated, const Vector4& axis, float radians) 
    {
        Matrix4x4 mat = RotateLocal(axis, radians);
        return rotated * mat;
    }
    
    Matrix4x4 Scale(const Matrix4x4& scaled, const Vector4& scale) 
    {
        return Matrix4x4
        (
            scale.x, 0, 0, 0,
            0, scale.y, 0, 0,
            0, 0, scale.z, 0,
            0, 0, 0,       1
        );
    }
    
    Matrix4x4 Transpose(const Matrix4x4& m) 
    {
        return Matrix4x4
        (
            m.x0, m.y0, m.z0, m.w0,
            m.x1, m.y1, m.z1, m.w1,
            m.x2, m.y2, m.z2, m.w2,
            m.x3, m.y3, m.z3, m.w3
        );
    }
   

    Quaternion Identity() 
    {
        return Quaternion(0, 0, 0, 1);
    }
    Quaternion Conjugate(const Quaternion& q) 
    {
        return Normalize(Quaternion(-q.x, -q.y, -q.z, q.w));
    }

    Quaternion FromEuler(float pitch, float yaw, float roll) 
    {
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);
        return Normalize(Quaternion(
            cy * sp * cr + sy * cp * sr,
            sy * cp * cr - cy * sp * sr,
            cy * cp * sr - sy * sp * cr,
            cy * cp * cr + sy * sp * sr
        ));
    }
    Quaternion FromAxisAngle(const Vector3& axis, float angle) 
    {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        return Normalize(Quaternion(
            axis.x * sinHalf,
            axis.y * sinHalf,
            axis.z * sinHalf,
            std::cos(halfAngle)
        ));
    }

    Matrix4x4 ToMatrix4x4(const Quaternion& q) 
    {
        float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
        float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
        float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

        return Matrix4x4(
            1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
            2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
            2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 Inverse(const Matrix4x4& m) 
    {
        if (true)  //TO DO своя реализация!!!
        {
            // Создаем GLM-матрицу из вашей матрицы (обратите внимание на порядок элементов!)
            //glm::mat4 matrix(
            //    m.x0, m.x1, m.x2, m.x3,  // Первая строка вашей матрицы
            //    m.y0, m.y1, m.y2, m.y3,  // Вторая строка
            //    m.z0, m.z1, m.z2, m.z3,  // Третья строка
            //    m.w0, m.w1, m.w2, m.w3   // Четвертая строка
            //);

            // Вычисляем обратную матрицу
            glm::mat4 inversed = glm::inverse(ToGLM(m));

            // Конвертируем обратно в вашу структуру
            return FromGLM(inversed);
            
        }
        return Matrix4x4();
    }

    Quaternion Inverse(const Quaternion& q)
    {
        float lenSq = LengthSquared(q);
        if (lenSq > std::numeric_limits<float>::epsilon()) {
            return Conjugate(q) * (1.0f / lenSq);
        }
        return Quaternion();
    }


    Matrix4x4 Perspective(float fovRadians, float aspect, float near, float far) {
        const float tanHalfFov = std::tan(fovRadians * 0.5f);
        const float yScale = 1.0f / tanHalfFov;
        const float xScale = yScale / aspect;

        return Matrix4x4(
            xScale, 0, 0, 0,
            0, yScale, 0, 0,
            0, 0, -(far + near) / (far - near), -2.0f * far * near / (far - near),
            0, 0, -1.0f, 0
        );
    }
    Matrix4x4 Ortho(float left, float right, float bottom, float top, float near, float far) {
        const float rl = right - left;
        const float tb = top - bottom;
        const float fn = far - near;

        return Matrix4x4(
            2.0f / rl, 0.0f, 0.0f, -(right + left) / rl,
            0.0f, 2.0f / tb, 0.0f, -(top + bottom) / tb,
            0.0f, 0.0f, -2.0f / fn, -(far + near) / fn,
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }

    Matrix4x4 Ortho2D(float left, float right, float bottom, float top)
    {
        return Ortho(left,right,bottom,top,-1.0f,1.0f);
    }

    Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
        Vector3 zaxis = Normalize(eye - target); // Направление "вперёд" (к цели)
        Vector3 xaxis = Normalize(Cross(up, zaxis)); // Вправо
        Vector3 yaxis = Cross(zaxis, xaxis); // Истинный "верх"

        return Matrix4x4(
            xaxis.x, xaxis.y, xaxis.z, -Dot(xaxis, eye),
            yaxis.x, yaxis.y, yaxis.z, -Dot(yaxis, eye),
            zaxis.x, zaxis.y, zaxis.z, -Dot(zaxis, eye),
            0, 0, 0, 1.0f
        );
    }
    
    glm::vec3 ToGLM(const Vector3& v) 
    {
        return glm::vec3(v.x, v.y, v.z);
    }
    glm::mat4 ToGLM(const Matrix4x4& m)
    {
        return glm::mat4(
            m.x0, m.x1, m.x2, m.x3,  // Первая строка вашей матрицы
            m.y0, m.y1, m.y2, m.y3,  // Вторая строка
            m.z0, m.z1, m.z2, m.z3,  // Третья строка
            m.w0, m.w1, m.w2, m.w3   // Четвертая строка
        );
    }
    Vector3 FromGLM(const glm::vec3& v) 
    {
        return Vector3(v.x, v.y, v.z);
    }
    Matrix4x4 FromGLM(const glm::mat4& m)
    {
        return Matrix4x4{
                m[0][0], m[1][0], m[2][0], m[3][0],  // Первый столбец GLM -> первая строка вашей
                m[0][1], m[1][1], m[2][1], m[3][1],  // Второй столбец GLM -> вторая строка
                m[0][2], m[1][2], m[2][2], m[3][2],  // Третий столбец
                m[0][3], m[1][3], m[2][3], m[3][3]   // Четвертый столбец
        };
    }
    Vector2 Min(const Vector2& v1, const Vector2& v2)
    {
        return Vector2((v1.x < v2.x) ? v1.x : v2.x,
            (v1.y < v2.y) ? v1.y : v2.y);
    }
    Vector3 Min(const Vector3& v1, const Vector3& v2)
    {
        return Vector3((v1.x < v2.x) ? v1.x : v2.x,
            (v1.y < v2.y) ? v1.y : v2.y,
            (v1.z < v2.z) ? v1.z : v2.z);
    }
    Vector4 Min(const Vector4& v1, const Vector4& v2)
    {
        return Vector4((v1.x < v2.x) ? v1.x : v2.x,
            (v1.y < v2.y) ? v1.y : v2.y,
            (v1.z < v2.z) ? v1.z : v2.z,
            (v1.w < v2.w) ? v1.w : v2.w);
    }
    Vector2 Max(const Vector2& v1, const Vector2& v2)
    {
        return Vector2((v1.x > v2.x) ? v1.x : v2.x,
            (v1.y > v2.y) ? v1.y : v2.y);
    }
    Vector3 Max(const Vector3& v1, const Vector3& v2)
    {
        return Vector3((v1.x > v2.x) ? v1.x : v2.x,
            (v1.y > v2.y) ? v1.y : v2.y,
            (v1.z > v2.z) ? v1.z : v2.z);
    }
    Vector4 Max(const Vector4& v1, const Vector4& v2)
    {
        return Vector4((v1.x > v2.x) ? v1.x : v2.x,
            (v1.y > v2.y) ? v1.y : v2.y,
            (v1.z > v2.z) ? v1.z : v2.z,
            (v1.w > v2.w) ? v1.w : v2.w);
    }
    Vector2 Abs(const Vector2& v)
    {
        return Vector2(std::abs(v.x), std::abs(v.y));
    }
    Vector3 Abs(const Vector3& v)
    {
        return Vector3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
    }
    Vector4 Abs(const Vector4& v)
    {
        return Vector4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
    }
}

