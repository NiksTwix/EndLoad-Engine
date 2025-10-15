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
		void ExtractTextures(const aiMaterial* aiMat, Graphics::MaterialData& material) const;
		std::string ProcessTexturePath(const std::string& raw_path) const;
	public:
		MeshImportResult LoadSingleMesh(const std::string& path) const;

		static AssimpLoader& Get() 
		{
			static AssimpLoader al;
			return al;
		}
	};
}