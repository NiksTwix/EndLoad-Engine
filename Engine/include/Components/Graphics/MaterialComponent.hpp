#pragma once
#include <Components/IRComponentService.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <Resources/ShaderResource.hpp>
#include <Resources/MaterialResource.hpp>
namespace Components 
{


	struct MaterialComponent 
	{
		Graphics::ShaderID shaderID;
		std::unordered_map<std::string, Graphics::UniformValue> uniforms;
		std::unordered_map<Graphics::TextureType, Resources::ResourceID> textures;
		std::unordered_map<Graphics::TextureType, Graphics::TextureID> texturesID;
		Resources::ResourceID shaderResource = Definitions::InvalidID;
		Resources::ResourceID materialResource = Definitions::InvalidID;

		bool isValid;  
	};

	class MaterialComponentService : public IRComponentService
	{
	public:
		void Init() override;
		void Update(ECS::EntitySpace* eSpace) override;
		void Shutdown() override;
		void SetResourceShaderData(MaterialComponent& component, Resources::ResourceID shader_resource) const;
		void SetResourceMaterialData(MaterialComponent& component, Resources::ResourceID material_resource, bool generate_textures = false) const;
		bool UpdateResourceState(ECS::EntityID entity) override;
	};
}