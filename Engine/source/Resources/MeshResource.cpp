#include <Resources\MeshResource.hpp>
#include <Services/Importing/AssimpLoader.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>
#include <Core\ServiceLocator.hpp>

namespace Resources
{
	MeshResource::~MeshResource()
	{
		Release();
	}
	bool MeshResource::Load(const std::string& path)
	{
		if (path == "") return false;
		if (m_state != ResourceState::NotLoaded) {
			Diagnostics::Logger::Get().SendMessage(
				"(MeshResource) Retrying to download the resource. Free up the current data first.",
				Diagnostics::MessageType::Error
			);
			return false;
		}
		auto mesh_data = Importing::AssimpLoader::Get().LoadSingleMesh(path);
		m_path = path;
		return SetData(mesh_data.mesh);
	}
	bool MeshResource::SetData(std::any data)
	{
		if (!ValidateData<Graphics::MeshData>(data)) {
			Diagnostics::Logger::Get().SendMessage(
				"MeshResource: Invalid data type provided.",
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
				"MeshResource: Failed to cast data: " + std::string(e.what()) + ".",
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

		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	//We can to it by GetRenderWindow, but it can create new problems, as example, 
		//initializing before first frame (m_render_window = nullptr)

		if (!render_window) return false;

		m_dataID = render_window->GetGraphicsDevice()->CreateMesh(m_data);
		m_state = ResourceState::Initialized;

		m_ownerWindow = render_window->GetID();

		return true;
	}
	bool MeshResource::Uninit()
	{
		if (m_dataID == Definitions::InvalidID || (m_state != ResourceState::Initialized && m_state != ResourceState::NeedReinit)) return false;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	

		if (!render_window || render_window->GetID() != m_ownerWindow) return false;
		render_window->GetGraphicsDevice()->DestroyMesh(m_dataID);

		m_dataID = Definitions::InvalidID;
		m_ownerWindow = Definitions::InvalidID;
		m_state = ResourceState::Loaded;
		return true;
	}
	void MeshResource::Release()
	{
		Uninit();	// ResourceManager also calls Uninit() before clearing, but we must provide resource clearing;

		m_data = Graphics::MeshData();

		m_state = ResourceState::NotLoaded;
	}
	std::string MeshResource::GetType() const
	{
		return "Mesh";
	}
}