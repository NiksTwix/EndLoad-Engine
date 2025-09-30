#pragma once
#include <Core\IServices.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
//assimp
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/texture.h>
#include <filesystem>

namespace Importing 
{
	struct MeshImportResult {
		Graphics::MeshData mesh;
		Graphics::MaterialData material;
		bool hasMaterial = false;

		// Add information about bones for skeleton animation
		//bool hasSkeleton = false;
		//std::vector<BoneData> bones;
	};

	class AssimpLoader
	{
	private:
		const aiScene* LoadAssimpScene(const std::string& path) const;

		Graphics::MaterialData ExtractMaterialData(const aiMaterial* material) const;

		Graphics::MeshData ExtractMeshGeometry(const aiMesh* ai_mesh) const;

		std::filesystem::path ProcessTexturePath(const std::string& raw_path);
	public:
		MeshImportResult LoadSingleMesh(const std::string& path) const;
		//Impote/load scene

		//Scenes::SceneID ImportScene(const std::string& path)
		/*
			Imports scene from file.
			Creates entities with meshes,materials and etc
			Loads resources
		*/

		static AssimpLoader& Get() 
		{
			static AssimpLoader al;
			return al;
		}

	};


}