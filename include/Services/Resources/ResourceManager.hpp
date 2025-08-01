#pragma once
#include <memory>
#include <unordered_map>
#include <Core/IServices.hpp>
#include <Resources/IResource.hpp>


/*TODO
* ��������� �������
* ����������� ��������
* ������������ ��������
* ����������
* ������� �����
*/


class ResourceManager : public IHelperService
{
public:

    // ��������� �����������
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

	ResourceManager() { Init(); }
	void Init() override;
	void Shutdown() override;

    template<typename T>
    std::shared_ptr<T> Load(std::string path) 
    {
        static_assert(std::is_base_of_v<IResource, T>, "T must inherit from IResource");
        
        // 1. ��������� ��� (����� ����������, ��� contains + operator[])
        if (auto it = m_resources.find(path); it != m_resources.end())
        {
            // 2. ���������� ���������� ����
            return std::dynamic_pointer_cast<T>(it->second);
        }

        // 3. ������� � ��������� ������
        auto resource = std::make_shared<T>();
        if (!resource->Load(path))
        {
            return nullptr;
        }

        // 4. ��������� � ���
        m_resources[path] = resource;
        return resource;

    }
    void Clear();
    ~ResourceManager() { Shutdown(); }

    template<typename T>
    T* Get(std::string path)
    {
        static_assert(std::is_base_of_v<IResource, T>, "T must inherit from IResource");

        if (auto it = m_resources.find(path); it != m_resources.end())
        {
            return static_cast<T*>(it->second.get());
        }

        return nullptr;
    }
         

private:
    std::unordered_map<std::string, std::shared_ptr<IResource>> m_resources; // Cashed [path, resource]
};

