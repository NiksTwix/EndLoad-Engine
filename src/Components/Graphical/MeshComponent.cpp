#include <Components\Graphical/MeshComponent.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Core/Logger.hpp>
#include <Services/Resources/ResourceManager.hpp>
#include <Resources/MeshResource.hpp>
#include <Systems/Render/Render.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>
#include <sstream>


MeshService::MeshService()
{
	m_stringType = "Mesh";
	m_ComponentType = typeid(MeshComponent);
}

void MeshService::Init()
{
	//auto serser = ServiceLocator::Get<SerializationService>();
    //if (serser) serser->Register<Mesh>(Serialize, Deserialize);
}

void MeshService::Shutdown()
{
}

MeshID MeshService::Load(const std::string& path)
{
	auto resource = ServiceLocator::Get<ResourceManager>()->Load<MeshResource>(path);
	if (!resource) return SIZE_MAX;
	MeshData data;

	data.indexCount = resource->GetIndexCount();
	data.vertexCount = resource->GetVertexCount();
	data.vao = resource->GetVAO();
	data.path = resource->GetPath();

	auto id = currentId;

	m_meshes[id] = data;

	return id;
}

void MeshService::Bind(MeshID id)
{	//TODO поддержка другой API, а не только OpenGL

	if (m_meshes.count(id) <= 0)
	{
		ServiceLocator::Get<Logger>()->Log("(MeshService): mesh id " + std::to_string(id) + " is invalid.", Logger::Level::Error);
		return;
	}
	ServiceLocator::Get<Render>()->GetContext()->DrawElements(m_meshes[id].vao, m_meshes[id].indexCount);

}

MeshComponent MeshService::CreateMesh(const std::string& path)
{
	MeshID meshid = Load(path);
	MeshComponent mesh;
	mesh.mesh_id = meshid;

	BuildAABB(mesh);
	return mesh;
}

MeshData& MeshService::GetData(MeshID id)
{
	return m_meshes[id];
}

void MeshService::BuildAABB(MeshComponent& mesh)
{
	MeshResource* meshr = ServiceLocator::Get<ResourceManager>()->Load<MeshResource>(GetData(mesh.mesh_id).path).get();

	if (meshr == nullptr) 
	{
		ServiceLocator::Get<Logger>()->Log("(MeshService::BuildAABB): mesh id " + std::to_string(mesh.mesh_id) + ", resource is invalid.", Logger::Level::Error);
		return;
	}

	for (auto vertex : meshr->GetVertices()) {
		if (vertex.position.x > mesh.MaxAABB.x) mesh.MaxAABB = vertex.position.x;
		if (vertex.position.y > mesh.MaxAABB.y) mesh.MaxAABB = vertex.position.y;
		if (vertex.position.z > mesh.MaxAABB.z) mesh.MaxAABB = vertex.position.z;

		if (vertex.position.x < mesh.MinAABB.x) mesh.MinAABB = vertex.position.x;
		if (vertex.position.y < mesh.MinAABB.y) mesh.MinAABB = vertex.position.y;
		if (vertex.position.z < mesh.MinAABB.z) mesh.MinAABB = vertex.position.z;
	}

	mesh.Size = mesh.MaxAABB - mesh.MinAABB;

	mesh.Center = (mesh.MinAABB + mesh.MaxAABB) * 0.5f;
}

AABB MeshService::GetTransformedAABB(const MeshComponent& mesh, const Math::Matrix4x4& modelMatrix)
{
	//OBB
	std::vector<Math::Vector3> corners =
	{
		Math::Vector3(mesh.MaxAABB.x, mesh.MaxAABB.y,mesh.MaxAABB.z),
		Math::Vector3(mesh.MinAABB.x, mesh.MaxAABB.y,mesh.MaxAABB.z),
		Math::Vector3(mesh.MinAABB.x, mesh.MinAABB.y,mesh.MaxAABB.z),
		Math::Vector3(mesh.MinAABB.x, mesh.MinAABB.y,mesh.MinAABB.z),
	
		Math::Vector3(mesh.MaxAABB.x, mesh.MaxAABB.y,mesh.MinAABB.z),
		Math::Vector3(mesh.MaxAABB.x, mesh.MinAABB.y,mesh.MinAABB.z),
		Math::Vector3(mesh.MaxAABB.x, mesh.MinAABB.y,mesh.MaxAABB.z),
		Math::Vector3(mesh.MinAABB.x, mesh.MaxAABB.y,mesh.MinAABB.z),
	};
	
	Math::Vector3 worldMin(FLT_MAX), worldMax(-FLT_MAX);
	for (const auto& corner : corners) {
		Math::Vector3 worldPos = (Math::Vector3)(modelMatrix * Math::Vector4(corner.x,corner.y,corner.z, 1.0f));
		worldMin = Math::Min(worldMin, worldPos);
		worldMax = Math::Max(worldMax, worldPos);
	}
	return AABB(worldMax, worldMin);
}

void MeshService::Update(ECS::EntitySpace* eSpace)
{
	m_eSpace = eSpace;
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> MeshService::GetComponentFields(void* component)
{
	std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
	if (component == nullptr) return result;
	MeshComponent* mesh = static_cast<MeshComponent*>(component);
	
	auto path = GetData(mesh->mesh_id).path;

	result["Path"] = path;


	return result;
}

void MeshService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
	MeshComponent mesh;

	for (auto [fieldName, fieldValue] : fields)
	{
		if (fieldName == "Path") {
			if (auto wu = ECS::ESDL::GetAs<std::string>(fieldValue)) {
				mesh = CreateMesh(*wu);
			}
			else {
				ServiceLocator::Get<Logger>()->Log("(MeshService): mesh's deserialization error: value's type is not string. Entity " + std::to_string(id) + ".", Logger::Level::Error);
			}
		}
	}
	if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<MeshComponent>(id, mesh);
	else
	{
		ServiceLocator::Get<Logger>()->Log("(MeshService): mesh's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
	}
}


