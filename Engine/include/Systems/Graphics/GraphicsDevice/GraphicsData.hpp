#pragma once
#include <SpecialHeaders\Definitions.hpp>
#include <ELMath\include\Vertex.hpp>
#include <ELMath\include\Vector4.hpp>
#include <ELMath\include\Matrix4x4.hpp>
// This file contains special structs for graphics

namespace Graphics
{
    
    using MeshID = uint;
    using TextureID = uint;
    using ShaderID = uint;


    struct UniformValue
    {
        enum class ValueType
        {
            BOOL,
            INT,
            UINT,
            FLOAT,
            VECTOR2,
            VECTOR3,
            VECTOR4,
            MATRIX4x4
        };

        union
        {
            bool boolValue;
            int intValue;
            uint uintValue;
            float floatValue;
        };

        Math::Vector4 vectorValue; // Can contain Vector2/Vector3 in  Vector40-style
        Math::Matrix4x4 matrixValue;

        ValueType type;

        // Constructor for every type
        UniformValue(bool value) : boolValue(value), type(ValueType::BOOL) {}
        UniformValue(int value) : intValue(value), type(ValueType::INT) {}
        UniformValue(uint value) : uintValue(value), type(ValueType::UINT) {}
        UniformValue(float value) : floatValue(value), type(ValueType::FLOAT) {}

        UniformValue(const Math::Vector2& value)
            : vectorValue(value.x, value.y, 0.0f, 0.0f), type(ValueType::VECTOR2) {
        }

        UniformValue(const Math::Vector3& value)
            : vectorValue(value.x, value.y, value.z, 0.0f), type(ValueType::VECTOR3) {
        }

        UniformValue(const Math::Vector4& value)
            : vectorValue(value), type(ValueType::VECTOR4) {
        }

        UniformValue(const Math::Matrix4x4& value)
            : matrixValue(value), type(ValueType::MATRIX4x4) {
        }

        // Default
        UniformValue() : floatValue(0.0f), type(ValueType::FLOAT) {}

        // Methods for save values getting 
        bool GetBool() const {
            assert(type == ValueType::BOOL);
            return boolValue;
        }
        int GetInt() const {
            assert(type == ValueType::INT);
            return intValue;
        }
        uint GetUInt() const {
            assert(type == ValueType::UINT);
            return uintValue;
        }

        float GetFloat() const {
            assert(type == ValueType::FLOAT);
            return floatValue;
        }
        Math::Vector2 GetVector2() const {
            assert(type == ValueType::VECTOR2);
            return Math::Vector2(vectorValue.x, vectorValue.y);
        }
        Math::Vector3 GetVector3() const {
            assert(type == ValueType::VECTOR3);
            return Math::Vector3(vectorValue.x, vectorValue.y, vectorValue.z);
        }
        Math::Vector4 GetVector4() const {
            assert(type == ValueType::VECTOR4);
            return vectorValue;
        }
        Math::Matrix4x4 GetMatrix4x4() const {
            assert(type == ValueType::MATRIX4x4);
            return matrixValue;
        }
    };


    struct MeshData
    {
        std::vector<Math::Vertex> vertices;
        std::vector<uint> indices;

        MeshData(std::vector<Math::Vertex> verts, std::vector<uint> inds)
            : vertices(std::move(verts))
            , indices(std::move(inds))
        {
        }
		uint GetVertexCount() const { return vertices.size(); }
		uint GetIndexCount() const { return indices.size(); }
	};

    struct TextureData
    {
        const void* pixels;
        Math::Vector2 size;
        uint bytes_per_pixel;

        TextureData(const void* pixels, Math::Vector2 size, uint bytes_per_pixel)
            : pixels(pixels)
            , size(size), bytes_per_pixel(bytes_per_pixel)
        {
        }
    };

    struct RenderCommand 
    {
        MeshID mesh_id;
        ShaderID shader_id;

        std::vector<std::pair<TextureID, uint>> textures; // Texture + slot (0-16)
        std::unordered_map<std::string, UniformValue> uniforms;
    };

    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION_CONTROL,    // Grid (ץו-ץ)
        TESSELATION_EVALUATION, // Grid (ץו-ץ)
        COMPUTE
        // ... and etc
    };

    struct ShaderSource {
        ShaderType type;
        std::string sourceCode; // Source code of GLSL/HLSL
        // We can add entry point (םאןנטלונ, "main")
    };

    class IShader {
    public:
        virtual ~IShader() = default;

        // Shader activation
        virtual void Bind() const = 0;
        // Method for uniform setting
        virtual void SetUniform(const std::string& name, const UniformValue& value) = 0;

        // Getters
        virtual ShaderID GetID() const = 0;
        virtual const std::string& GetName() const = 0;

        virtual bool Compile(const std::vector<ShaderSource>& sources) = 0;
    };
    using ShaderPtr = std::shared_ptr<IShader>;
}