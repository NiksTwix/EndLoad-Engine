#include <Resources\GLShader.hpp>
#include <Core\ServiceLocator.hpp>
#include <Core\Logger.hpp>
#include <fstream>
#include <sstream>


GLShader::~GLShader()
{
    Release();
}

GLShader::GLShader(const GLShader& other)
{
    // 1. Копируем исходный код шейдера
    m_vertexSrc = other.m_vertexSrc;
    m_fragmentSrc = other.m_fragmentSrc;

    // 2. Заново компилируем шейдер (создаём новый ID)
    if (!other.m_vertexSrc.empty() && !other.m_fragmentSrc.empty()) {
        LoadFromSource(m_vertexSrc, m_fragmentSrc);
    }
}

GLShader& GLShader::operator=(const GLShader& other)
{
    if (this != &other) {
        GLShader temp(other); // Используем конструктор копирования
        swap(*this, temp);    // Обмениваем содержимое
    }
    return *this;
}


void swap(GLShader& first, GLShader& second) noexcept
{
    using std::swap;
    swap(first.m_id, second.m_id);
    swap(first.m_vertexSrc, second.m_vertexSrc);
    swap(first.m_fragmentSrc, second.m_fragmentSrc);
}


bool GLShader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream vShaderFile(vertexPath), fShaderFile(fragmentPath);
    if (!vShaderFile.is_open() || !fShaderFile.is_open())
    {
        ServiceLocator::Get<Logger>()->Log("(GLShader): shader hasnt loaded: reading of shader file failed.", Logger::Level::Error);
        return false;
    }
    std::stringstream vStream, fStream;
    vStream << vShaderFile.rdbuf();
    fStream << fShaderFile.rdbuf();

    return LoadFromSource(vStream.str(), fStream.str());
}

bool GLShader::LoadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc) {
    if (vertexSrc.empty() || fragmentSrc.empty()) 
    {
        ServiceLocator::Get<Logger>()->Log("(GLShader): shader's sources are empty.", Logger::Level::Error);
        return false;
    }

    m_vertexSrc = vertexSrc;
    m_fragmentSrc = fragmentSrc;

    GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);

    GLint success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_id, 512, nullptr, infoLog);
        std::stringstream ss;
        ss << infoLog;
        ServiceLocator::Get<Logger>()->Log("(GLShader): shader linking failed: " + ss.str(), Logger::Level::Error);
        return false;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
}

void GLShader::Bind() const
{
    glUseProgram(m_id);
}

void GLShader::Release()
{
    glDeleteProgram(m_id);
}

int GLShader::SetUniform(const std::string& name, int value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform1i(id, (GLint)value);
    }
    return id;
}

int GLShader::SetUniform(const std::string& name, float value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform1f(id, (GLfloat)value);
    }
    return id;
}

int GLShader::SetUniform(const std::string& name, bool value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform1i(id, (GLint)value);
    }
    return id;
}

int GLShader::SetUniform(const std::string& name, const Math::Vector2& value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform2f(id, (GLfloat)value.x,(GLfloat)value.y);
    }
    return id;
}

int GLShader::SetUniform(const std::string& name, const Math::Vector3& value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform3f(id, (GLfloat)value.x, (GLfloat)value.y, (GLfloat)value.z);
    }
    return id;
}

int GLShader::SetUniform(const std::string& name, const Math::Vector4& value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniform4f(id, (GLfloat)value.x, (GLfloat)value.y, (GLfloat)value.z, (GLfloat)value.w);
    }
    return id;
}

//GLint GLShader::SetUniform(const std::string& name, const Math::Matrix2x2& value)
//{
//    GLint id = glGetUniformLocation(m_id, name.c_str());
//    if (id > -1)
//    {
//        glUniformMatrix2fv(id,1, GL_TRUE, ELMath::GetValuePtr(value));
//    }
//    return id;
//}
//
//GLint GLShader::SetUniform(const std::string& name, const ELMath::Matrix3x3& value)
//{
//    GLint id = glGetUniformLocation(m_id, name.c_str());
//    if (id > -1)
//    {
//        glUniformMatrix3fv(id, 1, GL_TRUE, ELMath::GetValuePtr(value));
//    }
//    return id;
//}

int GLShader::SetUniform(const std::string& name, const Math::Matrix4x4& value)
{
    GLint id = glGetUniformLocation(m_id, name.c_str());
    if (id > -1)
    {
        glUniformMatrix4fv(id, 1, GL_TRUE, Math::GetValuePtr(value));
    }
    return id;
}

std::pair<std::string, std::string> GLShader::GetShaderSources()
{
    return {m_vertexSrc, m_fragmentSrc};
}

unsigned int GLShader::CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::stringstream ss;
        ss << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
            << " shader compilation failed: " << infoLog;

        ServiceLocator::Get<Logger>()->Log(ss.str(), Logger::Level::Error);

        return 0;
    }

    return shader;
}

void GLShader::Unbind() const
{
    glUseProgram(0);
}


