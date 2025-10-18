#pragma once
#include <Resources/IResource.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>


namespace Resources 
{
	class TextureResource : public IResource 
	{
	public:
		bool Load(const std::string& path) override;
		bool SetData(std::any data) override;
		std::type_index GetDataType() const override;
		bool Init() override;
		bool Uninit() override;
		void Release() override;
		std::string GetType() const override;

		Graphics::TextureID GetDataID() const { return m_dataID; }

		static std::vector<unsigned char> LoadRaw(const std::string& path, Math::Vector2& resolution);

		const Graphics::TextureData& GetData() const { return m_data; }

	private:
		Graphics::TextureData m_data;
		Graphics::TextureID m_dataID = Definitions::InvalidID;
	};


}

