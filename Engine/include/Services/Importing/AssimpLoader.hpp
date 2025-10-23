#pragma once
#include <Core\IServices.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>

#include <ELDCL/include/DCL.hpp>

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

		Assimp::Importer* importer;

		Graphics::MaterialData ExtractMaterialData(const aiMaterial* material) const;

		Graphics::MeshData ExtractMeshGeometry(const aiMesh* ai_mesh) const;
		void ExtractTextures(const aiMaterial* aiMat, Graphics::MaterialData& material) const;
		std::string ProcessTexturePath(const std::string& raw_path) const;
	public:

		AssimpLoader() 
		{
			importer = new Assimp::Importer();
		}

		~AssimpLoader() 
		{
			importer->FreeScene();
			delete importer;
		}

		MeshImportResult LoadSingleMesh(const std::string& path) const;

		static AssimpLoader& Get() 
		{
			static AssimpLoader al;
			return al;
		}

		void FreeData() { if (importer)importer->FreeScene(); }

	};

	class AssimpELDCLConverter 
	{
	public:
		static AssimpELDCLConverter& Get()
		{
			static AssimpELDCLConverter al;
			return al;
		}

		std::shared_ptr<DCL::Container> MeshToEDCLBlock(MeshImportResult& data, bool base64 = false);

		std::shared_ptr<DCL::Container> MIRtoELDCLBlock(MeshImportResult& data, bool base64 = false);
		std::pair<std::string, std::string>  MIRtoELDCLText(MeshImportResult& data, bool base64 = false);	//Mesh and material

	};


}