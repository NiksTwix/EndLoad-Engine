#pragma once
#include <ELECS\ECS.hpp>
#include <ELMath/include/MathFunctions.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <Resources/ShaderResource.hpp>
#include <Resources/MaterialResource.hpp>
namespace Components 
{
	struct MaterialComponent 
	{
		Graphics::ShaderID shaderID;
		std::unordered_map<std::string, Graphics::UniformValue> uniforms;

		Resources::ResourceID shaderResource = Definitions::InvalidID;
		Resources::ResourceID materialResource = Definitions::InvalidID;

		//Albedo,Normal,Metallic,Roughness,Emission,Occlusion textures in uniforms. Replacing is going trought materialResource

		bool texturesInited;	//Flag for initializations of textures in InitResources
		/*
			Если флаг не установлен, то BuildCommand вызовет метод InitResources, что загрузит и инициализирует текстуры в текущей фрейм. 
			Проблема с деинициализацией не грозит, 5 минут бездействия и ресурс удаляется
		*/

		bool isValid;               
	};

	class MaterialComponentService : public ECS::IComponentService
	{
	public:
		void Init() override;
		void Update(ECS::EntitySpace* eSpace) override;
		void Shutdown() override;
		void SetResourceShaderData(MaterialComponent& component, Resources::ResourceID shader_resource) const;
		void SetResourceMaterialData(MaterialComponent& component, Resources::ResourceID material_resource) const;
		bool InitResources(MaterialComponent& component) const;
	};
}