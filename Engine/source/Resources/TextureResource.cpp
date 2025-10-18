#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>
#include <Services\ResourcesManager\ResourceManager.hpp>
#include <Resources/TextureResource.hpp>


namespace Resources 
{
	bool TextureResource::Load(const std::string& path)
	{
		Math::Vector2 size = Math::Vector2();
		auto result = LoadRaw(path, size);
		this->m_path = path;
		return SetData(Graphics::TextureData(result, size, 4));//data,size, bytes per pixel
	}
	bool TextureResource::SetData(std::any data)
	{
		if (!ValidateData<Graphics::TextureData>(data)) return false;
		m_data = std::any_cast<Graphics::TextureData>(data);
		m_state = ResourceState::Loaded;
		return true;
	}
	std::type_index TextureResource::GetDataType() const
	{
		return typeid(Graphics::TextureData);
	}
	bool TextureResource::Init()
	{
		if (m_state == ResourceState::Initialized) return true;
		if (m_state != ResourceState::Loaded && m_state != ResourceState::NeedReinit) return false;
		//Calls graphic device to set up mesh

		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());	//We can to it by GetRenderWindow, but it can create new problems, as example, 
		//initializing before first frame (m_render_window = nullptr)

		if (!render_window) return false;

		m_dataID = render_window->GetGraphicsDevice()->CreateTexture(m_data);
		m_state = ResourceState::Initialized;

		m_ownerWindow = render_window->GetID();

		return true;
	}
	bool TextureResource::Uninit()
	{
		if (m_dataID == Definitions::InvalidID || (m_state != ResourceState::Initialized && m_state != ResourceState::NeedReinit)) return false;
		auto wm = Core::ServiceLocator::Get<Windows::WindowsManager>();
		auto rm = Core::ServiceLocator::Get<ResourceManager>();
		if (!wm || !rm) return false;

		//Getting current render device/context

		auto render_window = wm->GetWindow(rm->GetActiveWindow());

		if (!render_window || render_window->GetID() != m_ownerWindow) return false;
		render_window->GetGraphicsDevice()->DestroyTexture(m_dataID);

		m_dataID = Definitions::InvalidID;
		m_ownerWindow = Definitions::InvalidID;
		m_state = ResourceState::Loaded;
		return true;
	}
	void TextureResource::Release()
	{
		Uninit();
		m_data = Graphics::TextureData();
		m_state = ResourceState::NotLoaded;
	}
	std::string TextureResource::GetType() const
	{
		return "Texture";
	}
	std::vector<unsigned char> TextureResource::LoadRaw(const std::string& path, Math::Vector2& resolution)
	{
		int x, y, comp;
		stbi_set_flip_vertically_on_load(true); //For openGL
		auto* image = stbi_load(path.c_str(), &x, &y, &comp, STBI_rgb_alpha);
		if (image == nullptr) return std::vector<unsigned char>();
		
		resolution.x = x;
		resolution.y = y;
		size_t dataSize = x * y * comp; // STBI_rgb_alpha = 4 байта на пиксель
		std::vector<unsigned char> imageCopy(image, image + dataSize);	
		stbi_image_free(image);
		return imageCopy;
	}
}