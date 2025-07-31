#pragma once
#include <vector>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp/postprocess.h>
#include <Resources\IResource.hpp>
#include <Math\MathFunctions.hpp>
#include <Core/GL.hpp>



class MeshResource : public IResource 
{
public:

    MeshResource();

    bool Load(const std::string& path) override;

    void Release() override;

    std::string GetType() const override  { return  "Mesh"; }
    GLuint GetVAO() const { return m_vao; }
    size_t GetIndexCount() const { return m_indices.size(); }
    size_t GetVertexCount() const { return m_vertices.size(); }
    std::vector<Math::Vertex>& GetVertices() { return m_vertices; }
    std::vector<GLuint>& GetIndices() { return m_indices; }

private:

    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh);
    void UploadToGPU();

    std::vector<Math::Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    GLuint m_vao = 0, m_vbo = 0, m_ebo = 0;
};
