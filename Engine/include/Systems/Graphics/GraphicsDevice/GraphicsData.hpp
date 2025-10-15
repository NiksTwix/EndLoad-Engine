#pragma once
#include <SpecialHeaders\Definitions.hpp>
#include <Vertex.hpp>
#include <Vector4.hpp>
#include <Matrix4x4.hpp>
// This file contains special structs for graphics

namespace Graphics
{
    enum class GraphicsAPI {
        None,
        OpenGL,
        Vulkan,
        //DirectX11,
        // ...
    };
    
    enum class GDSettings
    {
        DEPTH_TEST,
        BLEND,
        BLEND_MODE,
        POLYGON_MODE,
        UNPACK_ALIGNMENT
    };
    enum class GDSettingsValues
    {
        NONE = 0,
        FALSE,
        TRUE,
        //BLEND_MODE
        SRC_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        //POLYGON_MODE
        POINTS,
        LINES,
        FILL,

        //bytes

        ONE_BYTE,
        TREE_BYTES,
        FOUR_BYTES
    };

    enum class GDBufferModes
    {
        STATIC,
        DYNAMIC,
        STREAM
    };

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
            Definitions::uint uintValue;
            float floatValue;
        };

        Math::Vector4 vectorValue; // Can contain Vector2/Vector3 in  Vector40-style
        Math::Matrix4x4 matrixValue;

        ValueType type;

        // Constructor for every type
        UniformValue(bool value) : boolValue(value), type(ValueType::BOOL) {}
        UniformValue(int value) : intValue(value), type(ValueType::INT) {}
        UniformValue(Definitions::uint value) : uintValue(value), type(ValueType::UINT) {}
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
        Definitions::uint GetUInt() const {
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
        std::vector<Definitions::uint> indices;

        GDBufferModes buffer_mode;      // <- STATIC, DYNAMIC, STREAM


        MeshData(std::vector<Math::Vertex> verts, std::vector<Definitions::uint> inds, GDBufferModes buffer_mode = GDBufferModes::STATIC)
            : vertices(std::move(verts))
            , indices(std::move(inds)), buffer_mode(buffer_mode)
        {
        }
        MeshData() = default;

        Definitions::uint GetVertexCount() const { return vertices.size(); }
        Definitions::uint GetIndexCount() const { return indices.size(); }
	};
    struct MaterialData {
        std::string name;

        // PBR параметры
        Math::Vector4 baseColor = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        float metallic = 0.0f;
        float roughness = 1.0f;
        float emissionIntensity = 0.0f;
        Math::Vector3 emissiveColor = Math::Vector3(0.0f);

        // Текстуры (пути или base64 последовательность)
        std::string albedoTexture;
        std::string normalTexture;
        std::string metallicTexture;
        std::string roughnessTexture;
        std::string emissionTexture;
        std::string occlusionTexture;

        std::vector<uint8_t> albedoBinary;
        std::vector<uint8_t> normalBinary;
        std::vector<uint8_t> metallicBinary;
        std::vector<uint8_t> roughnessBinary;
        std::vector<uint8_t> emissionBinary;
        std::vector<uint8_t> occlusionBinary;

        // Flags
        bool doubleSided = false;
        bool alphaTest = false;
        float alphaCutoff = 0.5f;

        std::string textureMode = "Path"; //or Base64

    };

    struct TextureData
    {
        const void* pixels;
        Math::Vector2 size;
        Definitions::uint bytes_per_pixel;

        TextureData(const void* pixels, Math::Vector2 size, Definitions::uint bytes_per_pixel)
            : pixels(pixels)
            , size(size), bytes_per_pixel(bytes_per_pixel)
        {
        }
    };

    struct RenderCommand 
    {
        MeshID mesh_id;
        ShaderID shader_id;

        std::vector<std::pair<TextureID, Definitions::uint>> textures; // Texture + slot (0-16)
        std::unordered_map<std::string, UniformValue> uniforms;
    };

    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELATION_CONTROL,    // Grid (хе-х)
        TESSELATION_EVALUATION, // Grid (хе-х)
        COMPUTE
        // ... and etc
    };

    struct ShaderSource {
        ShaderType type;
        std::string sourceCode; // Source code of GLSL/HLSL
        // We can add entry point (as example, "main")
    };
}