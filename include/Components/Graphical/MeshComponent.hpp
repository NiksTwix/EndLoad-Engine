#pragma once
#include <unordered_map>
#include <string>
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <Components\Basic\AABB.hpp>
#include <ECS\ECS.hpp>


using EntityID = size_t;
using MeshID = size_t;

struct LocalTransformComponent;

struct MeshComponent
{
	MeshID mesh_id = SIZE_MAX;
	//AABB
	Math::Vector3 MaxAABB = Math::Vector3(-FLT_MAX); //Максимальные координаты в локальном пространстве
	Math::Vector3 MinAABB = Math::Vector3(FLT_MAX); ; //Минимальные координаты в локальном пространстве
	Math::Vector3 Size;	//MaxAABB - MinAABB
	Math::Vector3 Center; //(MinAABB + MaxAABB) * 0.5f
};




struct MeshData {
	unsigned int vao;
	size_t vertexCount, indexCount;
	std::string path;
};

class MeshService : public ECS::IComponentService
{
private:
	std::unordered_map<MeshID, MeshData> m_meshes;
	MeshID currentId = 0;
public:
	MeshService();
	virtual void Init() override;  // Опционально
	virtual void Shutdown() override;

	MeshID Load(const std::string& path);
	void Bind(MeshID id);

	MeshComponent CreateMesh(const std::string& path);

	MeshData& GetData(MeshID id);

	void BuildAABB(MeshComponent& mesh);

	AABB GetTransformedAABB(const MeshComponent& mesh, const Math::Matrix4x4& modelMatrix);


	// Унаследовано через IComponentService
	void Update(ECS::EntitySpace* eSpace) override;

	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;

	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;

};