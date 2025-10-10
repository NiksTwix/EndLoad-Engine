#pragma once
#include <vector>
#include <Resources\IResource.hpp>
#include <MathFunctions.hpp>
#include <Systems\Graphics\GraphicsDevice\GraphicsData.hpp>


namespace Resources 
{
    class MeshResource : public IResource
    {
    public:
        MeshResource();
        ~MeshResource();

        // Унаследовано через IResource
        bool Load(const std::string& path) override;

        bool SetData(std::any data) override;       //MeshData

        std::type_index GetDataType() const override;

        bool Init() override;

        bool Uninit() override;

        void Release() override;

        std::string GetType() const override;

        const Graphics::MeshData& GetMeshData() const { return m_data; }
        const Graphics::MeshID GetMeshID() const { return m_dataID; }
    protected:
        Graphics::MeshData m_data;
        Graphics::MeshID m_dataID = Definitions::InvalidID;
    };
}

