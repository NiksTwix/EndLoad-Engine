#include <Resources/MaterialResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <ELDCL\include\Loading\Loader.hpp>
#include <Services/Importing/Base64Decoder.hpp>
#include <Resources/TextureResource.hpp>


namespace Resources 
{
	bool MaterialResource::SetData(std::any data)
	{
		// Type-safe установка данных
		if (!ValidateData<Graphics::MaterialData>(data)) return false;
		m_data = std::any_cast<Graphics::MaterialData>(data);
		m_state = ResourceState::Loaded;
		return true;
	}
	bool MaterialResource::Init()
	{
        if (m_state == ResourceState::Initialized) return true;
        if (m_state != ResourceState::Loaded && m_state != ResourceState::NeedReinit) return false;
		m_state = ResourceState::Initialized;
		return true;
	}
	bool MaterialResource::Uninit()
	{
		m_state = ResourceState::Loaded;
		return true;
	}
    bool MaterialResource::Load(const std::string& path)
    {
        if (path == "") return false;
        if (m_state != ResourceState::NotLoaded) {
            Diagnostics::Logger::Get().SendMessage(
                "(MaterialResource) Retrying to download the resource. Free up the current data first.",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        auto ct = DCL::Loader::LoadFromFile(path);
        if (!ct)
        {
            Diagnostics::Logger::Get().SendMessage(
                "(MaterialResource) Loading of material failed. Path: \"" + path + "\".",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        auto vector_shd = ct->GetByTag("material");
        if (vector_shd.empty())
        {
            Diagnostics::Logger::Get().SendMessage(
                "(MaterialResource) File doesn't contain material. Path: \"" + path + "\".",
                Diagnostics::MessageType::Error
            );
            return false;
        }

        std::vector<std::string> field_names = {
            "Name", "Parameters", "Textures"
        };

        auto fields = vector_shd[0]->FindFields(field_names);

        // Создаем MaterialData
        Graphics::MaterialData material_data;

        // Загружаем имя материала
        if (fields["Name"] != nullptr && !fields["Name"]->value.strVal.empty()) {
            material_data.name = fields["Name"]->value.strVal;
        }
        std::vector<std::string> parameters = {
                "BaseColor", "EmissionColor", "Roughness", "Metallic",
                "Emission", "DoubleSided", "AlphaTest", "AlphaCutOff"
        };
        // Обрабатываем параметры
        if (fields["Parameters"] != nullptr && fields["Parameters"]->container) {
            auto params = fields["Parameters"]->container->FindFields(parameters);

            // Base Color (Albedo)
            if (params["BaseColor"] != nullptr && params["BaseColor"]->value.arrayVal) {
                auto& color_array = *params["BaseColor"]->value.arrayVal;
                if (color_array.size() >= 3) {
                    material_data.baseColor.x = color_array.at(0).numberVal;
                    material_data.baseColor.y = color_array.at(1).numberVal;
                    material_data.baseColor.z = color_array.at(2).numberVal;
                    material_data.baseColor.w = (color_array.size() > 3) ? color_array[3].numberVal : 1.0f;
                }
            }

            // Emission Color
            if (params["EmissionColor"] != nullptr && params["EmissionColor"]->value.arrayVal) {
                auto& emission_array = *params["EmissionColor"]->value.arrayVal;
                if (emission_array.size() >= 3) {
                    material_data.emissiveColor.x = emission_array.at(0).numberVal;
                    material_data.emissiveColor.y = emission_array.at(1).numberVal;
                    material_data.emissiveColor.z = emission_array.at(2).numberVal;
                }
            }

            // PBR параметры
            if (params["Roughness"] != nullptr) {
                material_data.roughness = params["Roughness"]->value.numberVal;
            }
            if (params["Metallic"] != nullptr) {
                material_data.metallic = params["Metallic"]->value.numberVal;
            }
            if (params["Emission"] != nullptr) {
                material_data.emissionIntensity = params["Emission"]->value.numberVal;
            }

            // Flags
            if (params["DoubleSided"] != nullptr) {
                material_data.doubleSided = params["DoubleSided"]->value.boolVal;
            }
            if (params["AlphaTest"] != nullptr) {
                material_data.alphaTest = params["AlphaTest"]->value.boolVal;
            }
            if (params["AlphaCutOff"] != nullptr) {
                material_data.alphaCutoff = params["AlphaCutOff"]->value.numberVal;
            }

        }


        // Обрабатываем текстуры
        if (fields["Textures"] != nullptr && fields["Textures"]->container) {
            auto textures = ct->GetByTag("texture", fields["Textures"]->container);
            
            for (auto texture : textures) 
            {
                std::string data_field = texture->FindField("Data")->value.ToString();

                std::string data_format = texture->FindField("Format")->value.ToString();
                auto resolution = texture->FindField("Resolution")->value.arrayVal;
                if (data_field == "" || data_format == "" || resolution->size() < 2) continue;

                std::vector<unsigned char> bytes;

                Graphics::TextureType type;

                Math::Vector2 resolution_v = Math::Vector2(resolution->at(0).numberVal, resolution->at(1).numberVal);

                if (data_format == "base64")
                {
                    bytes = Importing::Base64Decoder::Get().Decode(data_field);
                }
                if (data_format == "path")
                {
                    bytes = TextureResource::LoadRaw(data_field,resolution_v);
                }
                if (texture->name == "Albedo")  type = Graphics::TextureType::ALBEDO;
                else if (texture->name == "Normal")  type = Graphics::TextureType::NORMAL;
                else if (texture->name == "Metallic")  type = Graphics::TextureType::METALLIC;
                else if (texture->name == "Roughness")  type = Graphics::TextureType::ROUGHNESS;
                else if (texture->name == "Emission")  type = Graphics::TextureType::EMISSION;
                else if (texture->name == "Occlusion")  type = Graphics::TextureType::OCCLUSION;

                material_data.raw_textures[type] = Graphics::TextureData(bytes,resolution_v,4);
            }
        }

        // Сохраняем загруженные данные
        m_path = path;
        SetData(material_data);
        return true;
    }
	std::type_index MaterialResource::GetDataType() const 
	{
		return typeid(Graphics::MaterialData);
	}
	void MaterialResource::Release()
	{
        Uninit();
		m_data = Graphics::MaterialData();
		m_state = ResourceState::NotLoaded;
	}
	std::string MaterialResource::GetType() const
	{
		return "Material";
	}
}