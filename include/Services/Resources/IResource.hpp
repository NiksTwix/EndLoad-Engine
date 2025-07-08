#pragma once
#include <string>

// Базовый класс для всех ресурсов
class IResource {
public:
    virtual ~IResource() = default;
    virtual bool Load(const std::string& path) = 0;
    virtual void Release() = 0;
    virtual std::string GetType() const = 0;

    std::string GetPath() const { return m_path; }

protected:
    std::string m_path;
};

