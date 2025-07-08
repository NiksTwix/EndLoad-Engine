#pragma once
#include <string>
#include <unordered_map>
#include <Services\Resources\Graphics\Shader.hpp>
#include <Core/GL.hpp>
#include <memory>




class GLShader : public Shader//: ELShader or shader
{
public:
    GLShader() = default;
    ~GLShader();

    GLShader(const GLShader& other);

    // Оператор присваивания через copy-and-swap
    GLShader& operator=(const GLShader& other);

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) override;
    bool LoadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc) override;
    void Bind() const override;
    void Unbind() const override;
    void Release() override;

    bool IsValid() const override { return m_id != NULL; }

    int SetUniform(const std::string& name, bool value) override;
    int SetUniform(const std::string& name, int value) override;
    int SetUniform(const std::string& name, float value) override;
    int SetUniform(const std::string& name, const Math::Vector2& value) override;
    int SetUniform(const std::string& name, const Math::Vector3& value) override;
    int SetUniform(const std::string& name, const Math::Vector4& value) override;
    int SetUniform(const std::string& name, const Math::Matrix4x4& value) override;

    std::pair<std::string, std::string> GetShaderSources() override; //Vertex, Fragment

private:
    unsigned int CompileShader(GLenum type, const std::string& source);
    int GetUniformLocation(const std::string& name);


    friend void swap(GLShader& first, GLShader& second) noexcept;

    std::string m_vertexSrc;
    std::string m_fragmentSrc;

    GLuint m_id = 0;
    std::unordered_map<std::string, GLuint> m_uniformCache;

    
};