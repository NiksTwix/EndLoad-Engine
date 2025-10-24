#include <Resources/ShaderResource.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <ELDCL\include\Loading\Loader.hpp>
#include <Services\ResourcesManager\ResourcesManager.hpp>
#include <Core\ServiceLocator.hpp>
namespace Resources 
{
	bool ShaderResource::Load(const std::string& path)
	{
		if (m_state != ResourceState::NotLoaded) { 
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) Retrying to download the resource. Free up the current data first.",
				Diagnostics::MessageType::Error
			);
			return false; 
		}
		//Parsing with DCL (Data Containing Language) - its new step for ESDL
		std::shared_ptr<DCL::ContainersTree> ct = DCL::Loader::LoadFromFile(path);
		if (!ct) 
		{
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) Loading of shader's sources failed:\"" + path + "\".",
				Diagnostics::MessageType::Error
			);
			return false;
		}
		auto vector_shd = ct->GetByTag("shader");
		if (vector_shd.empty())
		{
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) File doesnt contain shader:\"" + path + "\".",
				Diagnostics::MessageType::Error
			);
			return false;
		}

		//Take the first shader
		std::vector<std::string> field_names = { "Language","Name"};
		auto shdf = vector_shd[0]->FindFields(field_names);
		
		auto codes = ct->GetByTag("sources",vector_shd[0]);
		for (auto code : codes) 
		{
			Graphics::ShaderSource source;
			auto type = code->FindField("Type");
			if (type) 
			{
				if (type->value.strVal == "vertex") source.type = Graphics::ShaderType::VERTEX;
				if (type->value.strVal == "fragment") source.type = Graphics::ShaderType::FRAGMENT;
			}
			else continue;
			auto code_ = code->FindField("Code");
			if (code_)
			{
				source.sourceCode = code_->value.strVal;
			}
			else continue;

			m_dataSources.push_back(source);
		}
		if (m_dataSources.empty()) 
		{
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) Invalid sources.",
				Diagnostics::MessageType::Error
			);
			return false;
		}

		m_path = path;
		m_resourceName = shdf["Name"] ? shdf["Name"]->value.strVal : "Shader";
		m_state = ResourceState::Loaded;
		return true;
	}

	bool ShaderResource::SetData(std::any data)
	{
		if (!ValidateData<std::vector<Graphics::ShaderSource>>(data)) {
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) Invalid data type provided.",
				Diagnostics::MessageType::Error
			);
			return false;
		}
		try {
			m_dataSources = std::any_cast<std::vector<Graphics::ShaderSource>>(data);
			m_state = ResourceState::Loaded;
			return true;
		}
		catch (const std::bad_any_cast& e) {
			Diagnostics::Logger::Get().SendMessage(
				"(ShaderResource) Failed to cast data: " + std::string(e.what()) + ".",
				Diagnostics::MessageType::Error
			);
			return false;
		}
		return true;
	}

	std::type_index ShaderResource::GetDataType() const
	{
		return typeid(std::vector<Graphics::ShaderSource>);
	}

	bool ShaderResource::Init()
	{
		if (m_state == ResourceState::Initialized) return true;
		if (m_state != ResourceState::Loaded && m_state != ResourceState::NeedReinit) return false;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourcesManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	//We can to it by GetRenderWindow, but it can create new problems, as example, 
		//initializing before first frame (m_render_window = nullptr)

		if (!render_window) return false;

		m_dataID = render_window->GetGraphicsDevice()->CreateShader(m_dataSources);
		m_state = ResourceState::Initialized;

		m_ownerWindow = render_window->GetID();
		return true;
	}

	bool ShaderResource::Uninit()
	{
		if (m_dataID == Definitions::InvalidID || (m_state != ResourceState::Initialized && m_state != ResourceState::NeedReinit)) return false;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourcesManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());

		if (!render_window || render_window->GetID() != m_ownerWindow) return false;
		render_window->GetGraphicsDevice()->DestroyShader(m_dataID);

		m_dataID = Definitions::InvalidID;
		m_ownerWindow = Definitions::InvalidID;
		m_state = ResourceState::Loaded;
		return true;
	}

	void ShaderResource::Release()
	{
		Uninit();	// ResourcesManager also calls Uninit() before clearing, but we must provide resource clearing;

		m_dataSources.clear();

		m_state = ResourceState::NotLoaded;
	}

	std::string ShaderResource::GetType() const
	{
		return "Shader";
	}
}

