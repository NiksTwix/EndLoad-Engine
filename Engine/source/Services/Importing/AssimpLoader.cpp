#include <Services/Importing/AssimpLoader.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>

namespace Importing 
{
    const aiScene* AssimpLoader::LoadAssimpScene(const std::string& path)const
    {
        Assimp::Importer importer;

        // ����� ������ ����� ������ ���������
        unsigned int flags =
            aiProcess_Triangulate |           // ��� ��������� � ������������
            aiProcess_GenSmoothNormals |      // ���������� ������� (����� ��� GenNormals)
            aiProcess_CalcTangentSpace |      // �������� ��� ������ ��������
            aiProcess_JoinIdenticalVertices | // ������������ ������
            aiProcess_ImproveCacheLocality |  // ����������� ��� ���� GPU
            aiProcess_RemoveRedundantMaterials | // �������� ������ ����������
            aiProcess_FindDegenerates |       // ������� ����������� ���������
            aiProcess_FindInvalidData |       // ��������� ���������
            aiProcess_GenUVCoords |           // ��������� UV ���� �����������
            aiProcess_TransformUVCoords |     // ��������� UV ���������
            aiProcess_FlipUVs |               // OpenGL UV orientation
            aiProcess_OptimizeMeshes        // ����������� �����
            ;            

        const aiScene* scene = importer.ReadFile(path, flags);

        if (!scene) {
            Diagnostics::Logger::Get().SendMessage(
                "(AssimpLoader) Failed to load scene: " + path +
                " Error: " + importer.GetErrorString(),
                Diagnostics::MessageType::Error
            );
            return nullptr;
        }

        if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            Diagnostics::Logger::Get().SendMessage(
                "(AssimpLoader) Scene is incomplete: " + path,
                Diagnostics::MessageType::Warning  // Warning, � �� Error - ����� ���� usable
            );
    
        }
        Diagnostics::Logger::Get().SendMessage(
            "(AssimpLoader) Successfully loaded scene: " + path +
            " Meshes: " + std::to_string(scene->mNumMeshes) +
            " Materials: " + std::to_string(scene->mNumMaterials),
            Diagnostics::MessageType::Info
        );

        return scene;
    }
    Graphics::MaterialData  AssimpLoader::ExtractMaterialData(const aiMaterial* aiMat) const {
        Graphics::MaterialData material;

        // ��� ���������
        aiString matName;
        if (aiMat->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
            material.name = matName.C_Str();
        }

        // ������� ���� (Albedo/Diffuse)
        aiColor4D baseColor;
        if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS ||
            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
            material.baseColor = Math::Vector4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        }

        // �������������
        float metallic;
        if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
            material.metallic = metallic;
        }

        // �������������
        float roughness;
        if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
            material.roughness = roughness;
        }

        // ������������
        aiColor3D emissive;
        if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
            material.emissiveColor = Math::Vector3(emissive.r, emissive.g, emissive.b);
        }

        // ��������
        ExtractTextures(aiMat, material);

        // �����
        int doubleSided;
        if (aiMat->Get(AI_MATKEY_TWOSIDED, doubleSided) == AI_SUCCESS) {
            material.doubleSided = (doubleSided != 0);
        }

        return material;
    }

    void AssimpLoader::ExtractTextures(const aiMaterial* aiMat, Graphics::MaterialData& material) const {
        // Albedo/Diffuse texture
        //if (aiMat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        //        material.raw_textures[Graphics::TextureType::ALBEDO = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //else if (aiMat->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_BASE_COLOR, 0, &path) == AI_SUCCESS) {
        //        material.albedoTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //
        //// Normal map
        //if (aiMat->GetTextureCount(aiTextureType_NORMALS) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
        //        material.normalTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //
        //// Metallic/Roughness (����� ���� ����������)
        //if (aiMat->GetTextureCount(aiTextureType_METALNESS) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS) {
        //        material.metallicTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //else if (aiMat->GetTextureCount(aiTextureType_UNKNOWN) > 0) {
        //    // ������ PBR �������� ����� � UNKNOWN
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_UNKNOWN, 0, &path) == AI_SUCCESS) {
        //        material.metallicTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //// Emissive
        //if (aiMat->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_EMISSIVE, 0, &path) == AI_SUCCESS) {
        //        material.emissionTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
        //
        //// Ambient Occlusion
        //if (aiMat->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0) {
        //    aiString path;
        //    if (aiMat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) == AI_SUCCESS) {
        //        material.occlusionTexture = ProcessTexturePath(path.C_Str());
        //    }
        //}
    }

    std::string AssimpLoader::ProcessTexturePath(const std::string& rawPath) const {
        // ������� ���� �� ���������� �����, �������������� � �������������
        std::filesystem::path path(rawPath);
        return path.filename().string(); // ��� ���� ������ ��������� �����
    }
    Graphics::MeshData AssimpLoader::ExtractMeshGeometry(const aiMesh* ai_mesh) const
    {
        std::vector<Math::Vertex> vertices;
        std::vector<Definitions::uint> indices;
        // ��������� �������
        vertices.reserve(ai_mesh->mNumVertices);
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
            Math::Vertex vertex;

            // �������
            vertex.position.x = ai_mesh->mVertices[i].x;
            vertex.position.y = ai_mesh->mVertices[i].y;
            vertex.position.z = ai_mesh->mVertices[i].z;

            // ������� (���� ����)
            if (ai_mesh->HasNormals()) {
                vertex.normal.x = ai_mesh->mNormals[i].x;
                vertex.normal.y = ai_mesh->mNormals[i].y;
                vertex.normal.z = ai_mesh->mNormals[i].z;
            }

            // ���������� ���������� (����� ������ UV �����)
            if (ai_mesh->HasTextureCoords(0)) {
                vertex.texCoord.x = ai_mesh->mTextureCoords[0][i].x;
                vertex.texCoord.y = ai_mesh->mTextureCoords[0][i].y;
            }

            // ��������/���������� (���� ����)
            if (ai_mesh->HasTangentsAndBitangents()) {
                vertex.tangent.x = ai_mesh->mTangents[i].x;
                vertex.tangent.y = ai_mesh->mTangents[i].y;
                vertex.tangent.z = ai_mesh->mTangents[i].z;

                vertex.bitangent.x = ai_mesh->mBitangents[i].x;
                vertex.bitangent.y = ai_mesh->mBitangents[i].y;
                vertex.bitangent.z = ai_mesh->mBitangents[i].z;
            }

            vertices.push_back(vertex);
        }

        // ��������� �������
        indices.reserve(ai_mesh->mNumFaces * 3); // ��� ���� �����������
        for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
            const aiFace& face = ai_mesh->mFaces[i];
            // Assimp ����������� ������������ ��-�� aiProcess_Triangulate
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        Graphics::MeshData meshData(vertices, indices);
        return meshData;
    }

    MeshImportResult AssimpLoader::LoadSingleMesh(const std::string& path) const {
        MeshImportResult result;

        const aiScene* scene = LoadAssimpScene(path);
        if (!scene || !scene->HasMeshes()) {
            return result;
        }

        // ����� ������ ���
        const aiMesh* ai_mesh = scene->mMeshes[0];

        // ��������� ���������
        result.mesh = ExtractMeshGeometry(ai_mesh);

        // ��������� ��������
        if (ai_mesh->mMaterialIndex >= 0) {
            const aiMaterial* ai_mat = scene->mMaterials[ai_mesh->mMaterialIndex];
            result.material = ExtractMaterialData(ai_mat);
            result.hasMaterial = true;
        }

        // ��������� �� ������� ������ (��� ��������)
        //result.hasSkeleton = (ai_mesh->HasBones());

        return result;
    }
}