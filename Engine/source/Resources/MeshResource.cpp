#include <Resources\MeshResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <ELDCL\include\Loading\Loader.hpp>
#include <Services/Importing/Base64Decoder.hpp>

namespace Resources
{
	MeshResource::~MeshResource()
	{
		Release();
	}
    bool MeshResource::Load(const std::string& path)
    {
        if (path == "") return false;
        if (m_state != ResourceState::NotLoaded) {
            Diagnostics::Logger::Get().SendMessage(
                "(MeshResource) Retrying to download the resource. Free up the current data first.",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        auto ct = DCL::Loader::LoadFromFile(path);
        if (!ct)
        {
            Diagnostics::Logger::Get().SendMessage(
                "(MeshResource) Loading of mesh failed. Path: \"" + path + "\".",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        auto vector_shd = ct->GetByTag("mesh");
        if (vector_shd.empty())
        {
            Diagnostics::Logger::Get().SendMessage(
                "(MeshResource) File dont contains mesh. Path: \"" + path + "\".",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        std::vector<std::string> field_names = {
            "DecodingFormat", "Name", "Vertices", "Indices", "Normals",
            "Tangents", "Bitangents", "VertexCount", "TextureCoordinates", "IndexCount", "HasTangents"
        };

        auto fields = vector_shd[0]->FindFields(field_names);

        // Получаем формат декодирования
        std::string decoding_format = "text";
        if (fields["DecodingFormat"] != nullptr && !fields["DecodingFormat"]->value.strVal.empty()) {
            decoding_format = fields["DecodingFormat"]->value.strVal;
        }

        // Получаем количество вершин и индексов
        Definitions::uint vertex_count = 0;
        Definitions::uint index_count = 0;

        if (fields["VertexCount"] != nullptr) {
            vertex_count = fields["VertexCount"]->value.numberVal;
        }
        if (fields["IndexCount"] != nullptr) {
            index_count = fields["IndexCount"]->value.numberVal;
        }

        // Декодируем основные данные
        std::vector<Math::Vector3> vertices;
        std::vector<Math::Vector3> normals;
        std::vector<Math::Vector2> texture_coords;
        std::vector<Definitions::uint> indices;

        if (fields["Vertices"] != nullptr && !fields["Vertices"]->value.strVal.empty()) {
            std::string vertices_data = fields["Vertices"]->value.strVal;
            if (decoding_format == "base64") {
                std::vector<float> raw_vertices = Importing::Base64Decoder::Get().DecodeFloatArray(vertices_data);
                if (raw_vertices.size() >= vertex_count * 3) {
                    vertices.reserve(vertex_count); 

                    for (size_t i = 0; i < vertex_count * 3; i += 3)  
                    {
                        Math::Vector3 current;
                        current.x = raw_vertices[i];
                        current.y = raw_vertices[i + 1];
                        current.z = raw_vertices[i + 2];
                        vertices.push_back(current);
                    }
                }
                else {
                    Diagnostics::Logger::Get().SendMessage(
                        "(MeshResource) Vertex data size mismatch. Expected: " +
                        std::to_string(vertex_count * 3) + ", got: " +
                        std::to_string(raw_vertices.size()),
                        Diagnostics::MessageType::Warning
                    );
                }
            }
            else {
                vertices = Math::FromStringToV3(vertices_data, ',');
            }
        }

        if (fields["Normals"] != nullptr && !fields["Normals"]->value.strVal.empty()) {
            std::string normals_data = fields["Normals"]->value.strVal;
            if (decoding_format == "base64") {
                std::vector<float> raw_normals = Importing::Base64Decoder::Get().DecodeFloatArray(normals_data);
                if (raw_normals.size() >= vertex_count * 3) {
                    normals.reserve(vertex_count);

                    for (size_t i = 0; i < vertex_count * 3; i += 3)
                    {
                        Math::Vector3 current;
                        current.x = raw_normals[i];
                        current.y = raw_normals[i + 1];
                        current.z = raw_normals[i + 2];
                        normals.push_back(current);
                    }
                }
                else {
                    Diagnostics::Logger::Get().SendMessage(
                        "(MeshResource) Normal data size mismatch. Expected: " +
                        std::to_string(vertex_count * 3) + ", got: " +
                        std::to_string(raw_normals.size()),
                        Diagnostics::MessageType::Warning
                    );
                }
            }
            else {
                normals = Math::FromStringToV3(normals_data, ',');
            }
        }

        if (fields["TextureCoordinates"] != nullptr && !fields["TextureCoordinates"]->value.strVal.empty()) {
            std::string texcoords_data = fields["TextureCoordinates"]->value.strVal;
            if (decoding_format == "base64") {
                std::vector<float> raw_tc = Importing::Base64Decoder::Get().DecodeFloatArray(texcoords_data);
                if (raw_tc.size() >= vertex_count * 2) {
                    texture_coords.reserve(vertex_count);

                    for (size_t i = 0; i < vertex_count * 2; i += 2)
                    {
                        Math::Vector2 current;
                        current.x = raw_tc[i];
                        current.y = raw_tc[i + 1];
                        texture_coords.push_back(current);
                    }
                }
                else {
                    Diagnostics::Logger::Get().SendMessage(
                        "(MeshResource) Texture coordinates size mismatch. Expected: " +
                        std::to_string(vertex_count * 2) + ", got: " +
                        std::to_string(raw_tc.size()),
                        Diagnostics::MessageType::Warning
                    );
                }
            }
            else {
                texture_coords = Math::FromStringToV2(texcoords_data, ',');
            }
        }

        if (fields["Indices"] != nullptr && !fields["Indices"]->value.strVal.empty()) {
            std::string indices_data = fields["Indices"]->value.strVal;
            if (decoding_format == "base64") {
                indices = Importing::Base64Decoder::Get().DecodeUIntArray(indices_data);
            }
            else {
                std::vector<int> sign_indices = Math::FromStringToInt(indices_data, ',');
                for (auto t : sign_indices) 
                {
                    indices.push_back(static_cast<Definitions::uint>(t));
                }
            }
        }

        // Декодируем тангенты и битангенты если есть
        std::vector<Math::Vector3> tangents;
        std::vector<Math::Vector3> bitangents;
        bool has_tangents = (fields["HasTangents"] != nullptr && fields["HasTangents"]->value.boolVal);

        if (has_tangents) {
            if (fields["Tangents"] != nullptr && !fields["Tangents"]->value.strVal.empty()) {
                std::string tangents_data = fields["Tangents"]->value.strVal;
                if (decoding_format == "base64") {
                    std::vector<float> raw_tangents = Importing::Base64Decoder::Get().DecodeFloatArray(tangents_data);
                    if (raw_tangents.size() >= vertex_count * 3) {
                        tangents.reserve(vertex_count);

                        for (size_t i = 0; i < vertex_count * 3; i += 3)
                        {
                            Math::Vector3 current;
                            current.x = raw_tangents[i];
                            current.y = raw_tangents[i + 1];
                            current.z = raw_tangents[i + 2];
                            tangents.push_back(current);
                        }
                    }
                    else {
                        Diagnostics::Logger::Get().SendMessage(
                            "(MeshResource) Tangents data size mismatch. Expected: " +
                            std::to_string(vertex_count * 3) + ", got: " +
                            std::to_string(raw_tangents.size()),
                            Diagnostics::MessageType::Warning
                        );
                    }
                }
                else {
                    tangents = Math::FromStringToV3(tangents_data, ',');
                }  
            }
            if (fields["Bitangents"] != nullptr && !fields["Bitangents"]->value.strVal.empty()) {
                std::string bitangents_data = fields["Bitangents"]->value.strVal;
                if (decoding_format == "base64") {
                    std::vector<float> raw_bitangents = Importing::Base64Decoder::Get().DecodeFloatArray(bitangents_data);
                    if (raw_bitangents.size() >= vertex_count * 3) {
                        raw_bitangents.reserve(vertex_count);

                        for (size_t i = 0; i < vertex_count * 3; i += 3)
                        {
                            Math::Vector3 current;
                            current.x = raw_bitangents[i];
                            current.y = raw_bitangents[i + 1];
                            current.z = raw_bitangents[i + 2];
                            bitangents.push_back(current);
                        }
                    }
                    else {
                        Diagnostics::Logger::Get().SendMessage(
                            "(MeshResource) Bitangents data size mismatch. Expected: " +
                            std::to_string(vertex_count * 3) + ", got: " +
                            std::to_string(raw_bitangents.size()),
                            Diagnostics::MessageType::Warning
                        );
                    }
                }
                else {
                    bitangents = Math::FromStringToV3(bitangents_data, ',');
                }
            }
        }

        if (vertices.size() != normals.size()) {
            Diagnostics::Logger::Get().SendMessage(
                "(MeshResource) Vertex and normal count mismatch! Vertices: " +
                std::to_string(vertices.size()) + ", Normals: " +
                std::to_string(normals.size()),
                Diagnostics::MessageType::Warning
            );
        }
        // Компонуем вершины
        std::vector<Math::Vertex> combined_vertices;
        combined_vertices.reserve(vertex_count);

        for (size_t i = 0; i < vertex_count; i++) {
            Math::Vertex vertex;

            vertex.position = (i < vertices.size()) ? vertices[i] : Math::Vector3();
            vertex.normal = (i < normals.size()) ? normals[i] : Math::Vector3();
            vertex.texCoord = (i < texture_coords.size()) ? texture_coords[i] : Math::Vector2();

            if (has_tangents) {
                vertex.tangent = (i < tangents.size()) ? tangents[i] : Math::Vector3();
                vertex.bitangent = (i < bitangents.size()) ? bitangents[i] : Math::Vector3();
            }
            else {
                vertex.tangent = Math::Vector3();
                vertex.bitangent = Math::Vector3();
            }

            combined_vertices.push_back(vertex);
        }


        // Создаем MeshData
        Graphics::MeshData mesh_data(
            std::move(combined_vertices),
            std::move(indices),
            Graphics::GDBufferModes::STATIC  // или другой режим по умолчанию
        );

        m_path = path;
        return SetData(mesh_data);
    }
	bool MeshResource::SetData(std::any data)
	{
		if (!ValidateData<Graphics::MeshData>(data)) {
			Diagnostics::Logger::Get().SendMessage(
				"MeshResource: Invalid data type provided.",
				Diagnostics::MessageType::Error
			);
			return false;
		}
		try {
			m_data = std::any_cast<Graphics::MeshData>(data);
			m_state = ResourceState::Loaded;
			return true;
		}
		catch (const std::bad_any_cast& e) {
			Diagnostics::Logger::Get().SendMessage(
				"MeshResource: Failed to cast data: " + std::string(e.what()) + ".",
				Diagnostics::MessageType::Error
			);
			return false;
		}
	}
	std::type_index MeshResource::GetDataType() const
	{
		return typeid(Graphics::MeshData());
	}
	bool MeshResource::Init()
	{
        if (m_state == ResourceState::Initialized) return true;
		//Calls graphic device to set up mesh
        
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	//We can to it by GetRenderWindow, but it can create new problems, as example, 
		//initializing before first frame (m_render_window = nullptr)

		if (!render_window) return false;

		m_dataID = render_window->GetGraphicsDevice()->CreateMesh(m_data);
		m_state = ResourceState::Initialized;

		m_ownerWindow = render_window->GetID();

		return true;
	}
	bool MeshResource::Uninit()
	{
		if (m_dataID == Definitions::InvalidID || (m_state != ResourceState::Initialized && m_state != ResourceState::NeedReinit)) return false;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	

		if (!render_window || render_window->GetID() != m_ownerWindow) return false;
		render_window->GetGraphicsDevice()->DestroyMesh(m_dataID);

		m_dataID = Definitions::InvalidID;
		m_ownerWindow = Definitions::InvalidID;
		m_state = ResourceState::Loaded;
		return true;
	}
	void MeshResource::Release()
	{
		Uninit();	// ResourceManager also calls Uninit() before clearing, but we must provide resource clearing;

		m_data = Graphics::MeshData();

		m_state = ResourceState::NotLoaded;
	}
	std::string MeshResource::GetType() const
	{
		return "Mesh";
	}
}