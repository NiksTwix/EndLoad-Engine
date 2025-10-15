#include <Resources/MaterialResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <ELDCL\include\Loading\Loader.hpp>
#include <Services/Importing/Base64Decoder.hpp>
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
                "AlbedoColor", "EmissionColor", "Roughness", "Metallic",
                "Emission", "DoubleSided", "AlphaTest", "AlphaCutOff"
        };
        // Обрабатываем параметры
        if (fields["Parameters"] != nullptr && fields["Parameters"]->container) {
            auto params = fields["Parameters"]->container->FindFields(parameters);

            // Base Color (Albedo)
            if (params["AlbedoColor"] != nullptr && params["AlbedoColor"]->value.arrayVal) {
                auto& color_array = *params["AlbedoColor"]->value.arrayVal;
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

        parameters = {
                "Albedo", "Normal", "Metallic", "Roughness", "Emission", "Occlusion"
        };

        // Обрабатываем текстуры
        if (fields["Textures"] != nullptr && fields["Textures"]->container) {
            auto textures = fields["Textures"]->container->FindFields(parameters);
            if (textures["TextureMode"] != nullptr) {
                material_data.textureMode = textures["TextureMode"]->value.strVal;
            }
            else 
            {
                material_data.textureMode = "path";
            }
            if (textures["Albedo"] != nullptr && !textures["Albedo"]->value.strVal.empty()) {
                material_data.albedoTexture = textures["Albedo"]->value.strVal;
                if (material_data.textureMode == "base64") 
                {
                    material_data.albedoBinary = Importing::Base64Decoder::Get().Decode(material_data.albedoTexture);
                    material_data.albedoTexture.clear();
                }
            }
            if (textures["Normal"] != nullptr && !textures["Normal"]->value.strVal.empty()) {
                material_data.normalTexture = textures["Normal"]->value.strVal;
                if (material_data.textureMode == "base64")
                {
                    material_data.normalBinary = Importing::Base64Decoder::Get().Decode(material_data.normalTexture);
                    material_data.normalTexture.clear();
                }
            }
            if (textures["Metallic"] != nullptr && !textures["Metallic"]->value.strVal.empty()) {
                material_data.metallicTexture = textures["Metallic"]->value.strVal;
                if (material_data.textureMode == "base64")
                {
                    material_data.metallicBinary = Importing::Base64Decoder::Get().Decode(material_data.metallicTexture);
                    material_data.metallicTexture.clear();
                }
            }
            if (textures["Roughness"] != nullptr && !textures["Roughness"]->value.strVal.empty()) {
                // Если нужно отдельное поле для roughness, можно добавить
                // material_data.roughnessTexture = textures["Roughness"]->value.strVal;
                /*
                if (material_data.textureMode == "base64") 
                {
                    material_data.roughnessBinary = Importing::Base64Decoder::Get().Decode(material_data.albedoTexture);
                    material_data.roughnessTexture.clear();
                }
                */
            }
            if (textures["Emission"] != nullptr && !textures["Emission"]->value.strVal.empty()) {
                material_data.emissionTexture = textures["Emission"]->value.strVal;
                if (material_data.textureMode == "base64")
                {
                    material_data.emissionBinary = Importing::Base64Decoder::Get().Decode(material_data.emissionTexture);
                    material_data.emissionTexture.clear();
                }
            }
            if (textures["Occlusion"] != nullptr && !textures["Occlusion"]->value.strVal.empty()) {
                material_data.occlusionTexture = textures["Occlusion"]->value.strVal;
                if (material_data.textureMode == "base64")
                {
                    material_data.occlusionBinary = Importing::Base64Decoder::Get().Decode(material_data.occlusionTexture);
                    material_data.occlusionTexture.clear();
                }
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
		m_data = Graphics::MaterialData();
		m_state = ResourceState::NotLoaded;
	}
	std::string MaterialResource::GetType() const
	{
		return "Material";
	}
}