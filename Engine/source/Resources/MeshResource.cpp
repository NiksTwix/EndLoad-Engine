#include <Resources\MeshResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>


namespace Resources
{
	bool MeshResource::Load(const std::string& path)
	{
		return false;
	}
	bool MeshResource::SetData(std::any data)
	{
		if (!ValidateData<Graphics::MeshData>(data)) {
			Diagnostics::Logger::Get().SendMessage(
				"MeshResource: Invalid data type provided",
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
				"MeshResource: Failed to cast data: " + std::string(e.what()),
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
		//Calls graphic device to set up mesh
		return false;
	}
	bool MeshResource::Uninit()
	{
		return false;
	}
	void MeshResource::Release()
	{
	}
	std::string MeshResource::GetType() const
	{
		return "Mesh";
	}
}