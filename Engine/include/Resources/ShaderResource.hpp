#pragma once
#include <Resources/IResource.hpp>
#include <Systems\Graphics\GraphicsDevice\GraphicsData.hpp>


namespace Resources 
{
	class ShaderResource : public IResource 
	{
	public:
		bool Load(const std::string& path) override;
		bool SetData(std::any data) override;	//std::vector<Graphics::ShaderSource>
		std::type_index GetDataType() const override;
		bool Init() override;
		bool Uninit() override;
		void Release() override;
		std::string GetType() const override;

		std::vector<Graphics::ShaderSource>& GetShaderSources() { return m_dataSources; };
		Graphics::ShaderID GetShaderID() const { return m_dataID; }
	private:
		std::vector<Graphics::ShaderSource> m_dataSources;
		Graphics::ShaderID m_dataID = Definitions::InvalidID;
	};

}