#include <Services/Importing/AssimpLoader.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>
#include <ELMath/include/MathFunctions.hpp>
#include <Services/Importing/Base64Decoder.hpp>
namespace Importing 
{
    const aiScene* AssimpLoader::LoadAssimpScene(const std::string& path)const
    {
        // Более полный набор флагов обработки
        unsigned int flags =
            aiProcess_Triangulate |           // Все примитивы в треугольники
            aiProcess_GenSmoothNormals |      // Сглаженные нормали (лучше чем GenNormals)
            aiProcess_CalcTangentSpace |      // Тангенты для нормал маппинга
            aiProcess_JoinIdenticalVertices | // Дедупликация вершин
            aiProcess_ImproveCacheLocality |  // Оптимизация для кеша GPU
            aiProcess_RemoveRedundantMaterials | // Удаление дублей материалов
            aiProcess_FindDegenerates |       // Находит вырожденные примитивы
            aiProcess_FindInvalidData |       // Валидация геометрии
            aiProcess_GenUVCoords |           // Генерация UV если отсутствуют
            aiProcess_TransformUVCoords |     // Коррекция UV координат
            aiProcess_FlipUVs |               // OpenGL UV orientation
            aiProcess_OptimizeMeshes        // Оптимизация мешей
            ;            

        const aiScene* scene = importer->ReadFile(path, flags);

        if (!scene) {
            Diagnostics::Logger::Get().SendMessage(
                "(AssimpLoader) Failed to load scene: " + path +
                " Error: " + importer->GetErrorString(),
                Diagnostics::MessageType::Error
            );
            return nullptr;
        }

        if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            Diagnostics::Logger::Get().SendMessage(
                "(AssimpLoader) Scene is incomplete: " + path,
                Diagnostics::MessageType::Warning  // Warning, а не Error - может быть usable
            );
    
        }
        Diagnostics::Logger::Get().SendMessage(
            "(AssimpLoader) Successfully loaded scene: " + path +
            " Meshes: " + std::to_string(scene->mNumMeshes) +
            " Materials: " + std::to_string(scene->mNumMaterials),
            Diagnostics::MessageType::Info
        );
        //importer.FreeScene();
        return scene;
    }
    Graphics::MaterialData  AssimpLoader::ExtractMaterialData(const aiMaterial* aiMat) const {
        Graphics::MaterialData material;

        // Имя материала
        aiString matName;
        if (aiMat->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
            material.name = matName.C_Str();
        }

        // Базовый цвет (Albedo/Diffuse)
        aiColor4D baseColor;
        if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS ||
            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor) == AI_SUCCESS) {
            material.baseColor = Math::Vector4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
        }

        // Металличность
        float metallic;
        if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
            material.metallic = metallic;
        }

        // Шероховатость
        float roughness;
        if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
            material.roughness = roughness;
        }

        // Эмиссивность
        aiColor3D emissive;
        if (aiMat->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS) {
            material.emissiveColor = Math::Vector3(emissive.r, emissive.g, emissive.b);
        }

        // Текстуры
        ExtractTextures(aiMat, material);

        // Флаги
        int doubleSided;
        if (aiMat->Get(AI_MATKEY_TWOSIDED, doubleSided) == AI_SUCCESS) {
            material.doubleSided = (doubleSided != 0);
        }

        return material;
    }

    void AssimpLoader::ExtractTextures(const aiMaterial* aiMat, Graphics::MaterialData& material) const {

    }

    std::string AssimpLoader::ProcessTexturePath(const std::string& rawPath) const {
        // Очистка пути от абсолютных путей, преобразование в относительные
        std::filesystem::path path(rawPath);
        return path.filename().string(); // или твоя логика обработки путей
    }
    Graphics::MeshData AssimpLoader::ExtractMeshGeometry(const aiMesh* ai_mesh) const
    {
        if (!ai_mesh || ai_mesh->mNumVertices == 0) {
            return Graphics::MeshData();
        }

        std::vector<Math::Vertex> vertices;
        std::vector<Definitions::uint> indices;

        // Извлекаем вершины
        vertices.reserve(ai_mesh->mNumVertices);
        for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++) {
            Math::Vertex vertex;

            // Позиция
            vertex.position.x = ai_mesh->mVertices[i].x;
            vertex.position.y = ai_mesh->mVertices[i].y;
            vertex.position.z = ai_mesh->mVertices[i].z;

            // Нормали (если есть)
            if (ai_mesh->HasNormals() && ai_mesh->mNormals) {
                vertex.normal.x = ai_mesh->mNormals[i].x;
                vertex.normal.y = ai_mesh->mNormals[i].y;
                vertex.normal.z = ai_mesh->mNormals[i].z;
            }

            // Текстурные координаты (берем первый UV набор)
            if (ai_mesh->HasTextureCoords(0) && ai_mesh->mTextureCoords[0]) {
                vertex.texCoord.x = ai_mesh->mTextureCoords[0][i].x;
                vertex.texCoord.y = ai_mesh->mTextureCoords[0][i].y;
            }

            // Тангенты/битангенты (если есть)
            if (ai_mesh->HasTangentsAndBitangents() && ai_mesh->mTangents && ai_mesh->mBitangents) {
                vertex.tangent.x = ai_mesh->mTangents[i].x;
                vertex.tangent.y = ai_mesh->mTangents[i].y;
                vertex.tangent.z = ai_mesh->mTangents[i].z;

                vertex.bitangent.x = ai_mesh->mBitangents[i].x;
                vertex.bitangent.y = ai_mesh->mBitangents[i].y;
                vertex.bitangent.z = ai_mesh->mBitangents[i].z;
            }

            vertices.push_back(vertex);
        }

        // Извлекаем индексы
        if (ai_mesh->mNumFaces > 0 && ai_mesh->mFaces) {
            indices.reserve(ai_mesh->mNumFaces * 3);

            for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++) {
                const aiFace& face = ai_mesh->mFaces[i];

                // Проверяем корректность данных грани
                if (!face.mIndices || face.mNumIndices == 0) {
                    continue;
                }

                // Проверяем, что индексы в допустимом диапазоне
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    if (face.mIndices[j] < ai_mesh->mNumVertices) {
                        indices.push_back(face.mIndices[j]);
                    }
                    else {
                        // Логируем ошибку или обрабатываем некорректный индекс
                        continue;
                    }
                }
            }
        }

        return Graphics::MeshData(vertices, indices);
    }

    MeshImportResult AssimpLoader::LoadSingleMesh(const std::string& path) const {
        MeshImportResult result;

        const aiScene* scene = LoadAssimpScene(path);
        if (!scene || !scene->HasMeshes()) {
            return result;
        }

        // Берем первый меш
        const aiMesh* ai_mesh = scene->mMeshes[0];
        if (!ai_mesh) return result;
        // Извлекаем геометрию
        result.mesh = ExtractMeshGeometry(ai_mesh);

        // Извлекаем материал
        if (ai_mesh->mMaterialIndex >= 0) {
            const aiMaterial* ai_mat = scene->mMaterials[ai_mesh->mMaterialIndex];
            result.material = ExtractMaterialData(ai_mat);
            result.hasMaterial = true;
        }

        // Проверяем на наличие костей (для анимации)
        //result.hasSkeleton = (ai_mesh->HasBones());

        return result;
    }
    std::shared_ptr<DCL::Container> AssimpELDCLConverter::MeshToEDCLBlock(MeshImportResult& data, bool base64)
    {
        std::shared_ptr<DCL::Container> mesh = std::make_shared<DCL::Container>();

        mesh->tag = "mesh";
        mesh->name = "Mesh";

        DCL::Field decfield("DecodingFormat", base64 ? "base64" : "text");

        mesh->ordered_fields.push_back(decfield);

        std::vector<float> positions_raw_data;
        std::vector<float> normals_raw_data;
        std::vector<float> bitangens_raw_data;
        std::vector<float> tangens_raw_data;
        std::vector<float> textcoords_raw_data;

        bool has_tangent = true;
        bool has_bitangent = true;
        for (auto& v : data.mesh.vertices)
        {
            auto array = v.position.ToArray();
            positions_raw_data.insert(positions_raw_data.end(), array.begin(), array.end());
            array = v.normal.ToArray();
            normals_raw_data.insert(normals_raw_data.end(), array.begin(), array.end());
            array = v.texCoord.ToArray();
            textcoords_raw_data.insert(textcoords_raw_data.end(), array.begin(), array.end());
            array = v.tangent.ToArray();
            tangens_raw_data.insert(tangens_raw_data.end(), array.begin(), array.end());
            array = v.bitangent.ToArray();
            bitangens_raw_data.insert(bitangens_raw_data.end(), array.begin(), array.end());
            if (has_tangent && Math::LengthSquared(v.tangent) == 0) has_tangent = false;
            if (has_bitangent && Math::LengthSquared(v.bitangent) == 0) has_bitangent = false;
        }
        //Bitangents or tangents are invalid
        if (!has_bitangent) bitangens_raw_data.clear();
        if (!has_tangent) tangens_raw_data.clear();
        mesh->ordered_fields.push_back(DCL::Field("HasBitangents", has_bitangent));
        mesh->ordered_fields.push_back(DCL::Field("Tangents", has_tangent));
        if (base64)
        {
            std::string coded_v = Base64Decoder::Get().CodeFloatArray(positions_raw_data);
            std::string coded_n = Base64Decoder::Get().CodeFloatArray(normals_raw_data);
            std::string coded_t = Base64Decoder::Get().CodeFloatArray(textcoords_raw_data);

            std::string coded_ta = Base64Decoder::Get().CodeFloatArray(tangens_raw_data);
            std::string coded_bta = Base64Decoder::Get().CodeFloatArray(bitangens_raw_data);
            std::string coded_i = Base64Decoder::Get().CodeUIntArray(data.mesh.indices);
            mesh->ordered_fields.push_back(DCL::Field("Positions", coded_v));
            mesh->ordered_fields.push_back(DCL::Field("Normals", coded_n));
            mesh->ordered_fields.push_back(DCL::Field("TextureCoordinates", coded_t));
            mesh->ordered_fields.push_back(DCL::Field("Tangents", coded_ta));
            mesh->ordered_fields.push_back(DCL::Field("Bitangents", coded_bta));
            mesh->ordered_fields.push_back(DCL::Field("Indices", coded_i));
            mesh->ordered_fields.push_back(DCL::Field("IndexCount", (int)data.mesh.GetIndexCount()));
            mesh->ordered_fields.push_back(DCL::Field("VertexCount", (int)data.mesh.GetVertexCount()));
        }
        if (!base64)
        {
            std::string coded_v;
            for (float v : positions_raw_data)
            {
                coded_v += std::to_string(v);
                coded_v += ",";
            }
            if (coded_v.size() > 0) coded_v.pop_back();

            std::string coded_n;
            for (float v : normals_raw_data)
            {
                coded_n += std::to_string(v);
                coded_n += ",";
            }
            if (coded_n.size() > 0) coded_n.pop_back();
            std::string coded_t;
            for (float v : textcoords_raw_data)
            {
                coded_t += std::to_string(v);
                coded_t += ",";
            }
            if (coded_t.size() > 0) coded_t.pop_back();
            std::string coded_ta;
            for (float v : tangens_raw_data)
            {
                coded_ta += std::to_string(v);
                coded_ta += ",";
            }
            if (coded_ta.size() > 0) coded_ta.pop_back();
            std::string coded_bta;
            for (float v : bitangens_raw_data)
            {
                coded_bta += std::to_string(v);
                coded_bta += ",";
            }
            if (coded_bta.size() > 0) coded_bta.pop_back();
            std::string coded_i;
            for (Definitions::uint v : data.mesh.indices)
            {
                coded_i += std::to_string(v);
                coded_i += ",";
            }
            if (coded_i.size() > 0) coded_i.pop_back();
            mesh->ordered_fields.push_back(DCL::Field("Positions", coded_v));
            mesh->ordered_fields.push_back(DCL::Field("Normals", coded_n));
            mesh->ordered_fields.push_back(DCL::Field("TextureCoordinates", coded_t));
            mesh->ordered_fields.push_back(DCL::Field("Tangents", coded_ta));
            mesh->ordered_fields.push_back(DCL::Field("Bitangents", coded_bta));
            mesh->ordered_fields.push_back(DCL::Field("Indices", coded_i));
            mesh->ordered_fields.push_back(DCL::Field("IndexCount", (int)data.mesh.GetIndexCount()));
            mesh->ordered_fields.push_back(DCL::Field("VertexCount", (int)data.mesh.GetVertexCount()));
        }
        
        return mesh;
    }
    std::shared_ptr<DCL::Container> AssimpELDCLConverter::MIRtoELDCLBlock(MeshImportResult& data, bool base64)
    {
        std::shared_ptr<DCL::Container> container = std::make_shared<DCL::Container>();
        auto mesh = MeshToEDCLBlock(data, base64);
        container->ordered_fields.push_back(DCL::Field("Mesh", mesh));
        //MATERIAL
        std::shared_ptr<DCL::Container> material = std::make_shared<DCL::Container>();




        return container;
    }
    std::pair<std::string, std::string> AssimpELDCLConverter::MIRtoELDCLText(MeshImportResult& data, bool base64)
    {
        std::pair<std::string, std::string> result;

        auto container = MIRtoELDCLBlock(data, base64);

        auto* mesh_field = container->FindField("Mesh");
        if (mesh_field) result.first = DCL::Serializator::Get().Serialize(mesh_field->container);

        return result;
    }
}