#include <Resources/ShaderResource.hpp>
#include <Services/Diagnostics/Logger/Logger.hpp>

namespace Resources 
{
	bool ShaderResource::Load(const std::string& path)
	{

		//Parsing with DCL (Data Containing Language) - its new step for ESDL



		return false;
	}

	bool ShaderResource::SetData(std::any data)
	{
		if (!ValidateData<std::vector<Graphics::ShaderSource>>(data)) {
			Diagnostics::Logger::Get().SendMessage(
				"MeshResource: Invalid data type provided",
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
				"MeshResource: Failed to cast data: " + std::string(e.what()),
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
		return false;
	}

	bool ShaderResource::Uninit()
	{
		return false;
	}

	void ShaderResource::Release()
	{
	}

	std::string ShaderResource::GetType() const
	{
		return "Shader";
	}
}

