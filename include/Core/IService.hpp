#pragma once


class IService 
{
public:
    IService() = default;
    virtual ~IService() = default;

    IService(const IService&) = delete;
    IService& operator=(const IService&) = delete;


    virtual void Init() { if (m_isValid) return; m_isValid = true; }  // Опционально
    virtual void Shutdown() { m_isValid = false; }

    bool IsValid() const { return m_isValid; }
    virtual bool IsNeedAsync() const { return false; }
protected:
    bool m_isValid = false;
};

