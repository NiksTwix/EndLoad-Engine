#include <Resources\MeshResource.hpp>
#include <Core/Logger.hpp>
#include <Core/ServiceLocator.hpp>
#include <Systems\Render\Render.hpp>
#include <sstream>

MeshResource::MeshResource()
{
    //if (!glewInit())
    //{
    //    ELServiceLocator::Get<ELLogger>()->Log("Glew hasnt inited or window context is invalid.", ELLogger::Level::Error);
    //}
}

bool MeshResource::Load(const std::string& path)
{
    m_path = path;

    

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | 
        aiProcess_GenNormals |
        aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        ServiceLocator::Get<Logger>()->Log("(MeshResource): Mesh on path:" + path + " hasnt loaded. Or scene data structure is incomplete", Logger::Level::Error);
        return false;
    }
    ProcessNode(scene->mRootNode, scene);
    UploadToGPU();
    return true;
}

void MeshResource::Release()
{
    ServiceLocator::Get<Render>()->GetContext()->DestroyMesh(m_vao);
}

void MeshResource::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (unsigned i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh);
    }
    for (unsigned i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void MeshResource::ProcessMesh(aiMesh* mesh)
{
    // Вершины
    for (unsigned i = 0; i < mesh->mNumVertices; i++) {
        Math::Vertex vertex;
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
        vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
        if (mesh->mTextureCoords[0]) {
            vertex.texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }
        m_vertices.push_back(vertex);
    }

    // Индексы
    for (unsigned i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; j++) {
            m_indices.push_back(face.mIndices[j]);
        }
    }
}

void MeshResource::UploadToGPU()
{
    //TODO поддержка другой API, а не только OpenGL

    this->m_vao = ServiceLocator::Get<Render>()->GetContext()->SetupMesh(m_vertices, m_indices);

    //glGenVertexArrays(1, &this->m_vao);
    //glGenBuffers(1, &this->m_vbo);
    //glGenBuffers(1, &this->m_ebo);
    //
    //glBindVertexArray(m_vao);
    //
    //glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    //glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Math::Vertex), m_vertices.data(), GL_STATIC_DRAW);
    //
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);
    //
    //// Атрибуты вершин
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, position));
    //
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, normal));
    //
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Math::Vertex), (void*)offsetof(Math::Vertex, texCoord));
    //
    //glBindVertexArray(0);
    //
    //auto error = glGetError();
    //
    //if (error != 0) 
    //{
    //    std::stringstream ss;
    //    ss << glewGetErrorString(error);
    //    ServiceLocator::Get<Logger>()->Log("Mesh upload to gpu error:" + ss.str(), Logger::Level::Error);
    //}
}
