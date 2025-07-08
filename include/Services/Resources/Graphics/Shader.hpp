#pragma once
#include <string>
#include <Math\MathFunctions.hpp>


class ShaderResource;

class Shader 
{
protected:

    friend ShaderResource;

	std::string m_name;
    std::string m_vertexSrc;
    std::string m_fragmentSrc;
    
    std::string m_path;

    //virtual unsigned int CompileShader(GLenum type, const std::string& source) = 0;
    //virtual int GetUniformLocation(const std::string& name) = 0;
public:
    Shader() = default;

    std::string GetName() { return m_name; }
    std::string GetPath() { return m_path; }
    void SetName(const std::string& newName) { m_name = newName; }
    virtual bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) = 0;
    virtual bool LoadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc) = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void Release() = 0;;
    virtual bool IsValid() const = 0;
    // ”ниформы
    virtual int SetUniform(const std::string& name, bool value) = 0;;
    virtual int SetUniform(const std::string& name, int value) = 0;;
    virtual int SetUniform(const std::string& name, float value) = 0;;
    virtual int SetUniform(const std::string& name, const Math::Vector2& value) = 0;;
    virtual int SetUniform(const std::string& name, const Math::Vector3& value) = 0;;
    virtual int SetUniform(const std::string& name, const Math::Vector4& value) = 0;;
    //GLint SetUniform(const std::string& name, const Math::Matrix2x2& value);
    //GLint SetUniform(const std::string& name, const Math::Matrix3x3& value);
    virtual int SetUniform(const std::string& name, const Math::Matrix4x4& value) = 0;;
    // ... другие типы
    virtual std::pair<std::string, std::string> GetShaderSources() = 0;; //Vertex, Fragment
};