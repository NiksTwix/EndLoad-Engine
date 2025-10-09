#pragma once
#include <Resources\IResource.hpp>
#include <Systems/Graphics/GraphicsDevice/IGraphicsDevice.hpp>


namespace Resources 
{
    class MaterialResource : public IResource {
    private:
        Graphics::MaterialData m_data;

    public:
        bool SetData(std::any data) override;

        bool Init() override;

        bool Uninit() override;

        const Graphics::MaterialData& GetData() const { return m_data; }

        // ������������ ����� IResource
        bool Load(const std::string& path) override;
        std::type_index GetDataType() const override;
        void Release() override;
        std::string GetType() const override;
    };
}

/*

class MaterialResource : public IResource {
private:
    MaterialData m_data;

public:
    bool SetData(std::any data) override {
        // Type-safe ��������� ������
        if (!ValidateData<MaterialData>(data)) return false;
        m_data = std::any_cast<MaterialData>(data);
        m_state = ResourceState::Loaded;
        return true;
    }

    bool Init() override {
        // ˨���� ������������� - ������ �������� ������
        if (m_data.metallic < 0 || m_data.metallic > 1) return false;
        if (m_data.roughness < 0 || m_data.roughness > 1) return false;
        m_state = ResourceState::Initialized;
        return true;
    }

    bool Uninit() override {
        // �� ������� ��������! ������ ���� ������
        m_state = ResourceState::Loaded;
        return true;
    }

    const MaterialData& GetData() const { return m_data; }
};
*/

/*
struct MaterialComponent {
    ResourceID materialDataId;  // ������ ��������� (������� ����, ���������)
    ResourceID albedoTextureId; // �������� ��� ��������� �������
    ResourceID normalTextureId;
    ResourceID metallicRoughnessTextureId;

    // ��� ��� ��������� (�����������)
    mutable ShaderProgram* cachedShader = nullptr;

    // ������� �������
    const MaterialData* GetMaterialData() const {
        return ResourceManager::Get()->GetResource<MaterialResource>(materialDataId);
    }

    const TextureResource* GetAlbedoTexture() const {
        return ResourceManager::Get()->GetResource<TextureResource>(albedoTextureId);
    }
};
*/