#include <Resources\FontResource.hpp>
#include <Services\Diagnostics\Logger\Logger.hpp>
#include <Core\ServiceLocator.hpp>
#include <Systems\Graphics\Windows\WindowsManager.hpp>

namespace Resources 
{
	bool Resources::FontResource::Init()
	{
		if (GenerateBitmap()) m_state = ResourceState::Initialized;
		else 
		{
			Diagnostics::Logger::Get().SendMessage("(FontResource) Error of bitmap generation.", Diagnostics::MessageType::Error);
			return false;
		}
		return true;
	}

	FT_Library* FontResource::GetFTLib()
	{
		static FT_Library ftLib = nullptr;
		static std::once_flag initFlag;

		auto freeFTLib = []() {
			if (ftLib) {
				FT_Done_FreeType(ftLib);
				ftLib = nullptr;
			}
			};

		std::call_once(initFlag, [&]() {
			if (FT_Init_FreeType(&ftLib)) {
				Diagnostics::Logger::Get().SendMessage("(FontResource) FT library isnt inited.", Diagnostics::MessageType::Error);
				ftLib = nullptr;
			}
			else {
				std::atexit(freeFTLib);  
			}
			});

		return &ftLib;
	}

	FontResource::FontResource()
	{
		m_resourceName = "Font";
	}

	bool FontResource::Load(const std::string& path)
	{
		auto ft = *GetFTLib();

		if (FT_New_Face(ft, path.c_str(), 0, &face)) {
			Diagnostics::Logger::Get().SendMessage("(FontResource) Font loading error.", Diagnostics::MessageType::Error);
			m_state = ResourceState::NotLoaded;
			return false;
		}

		SetSize(STANDART_FONT_SIZE);
		m_state = ResourceState::Loaded;
		return true;
	}

	void FontResource::Release()
	{
		FT_Done_Face(face);
		m_state = ResourceState::NotLoaded;
	}

	void FontResource::SetSize(unsigned int size)
	{
		if (!face) return;

		FT_Set_Pixel_Sizes(face, 0, size);
		this->size = size;
		m_state = ResourceState::NeedReinit;	//Change Init->Reinit
	}

	bool FontResource::GenerateBitmap()
	{
		if (!face || Core::ServiceLocator::Get<Windows::WindowsManager>()->GetRenderWindow() == nullptr) return false;
		FT_UInt glyphIndex;
		char32_t charCode = FT_Get_First_Char(face, &glyphIndex);

		Core::ServiceLocator::Get<Windows::WindowsManager>()->GetRenderWindow()->GetGraphicsDevice()->Set(Graphics::GDSettings::UNPACK_ALIGNMENT, Graphics::GDSettingsValues::ONE_BYTE);
		Uninit();	
		while (glyphIndex != 0) {
			bool isSupported = false;
			for (const auto& range : SUPPORTED_RANGES) {
				if (charCode >= range.first && charCode <= range.second) {
					isSupported = true;
					break;
				}
			}

			if (isSupported) {
				if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER)) {
					Diagnostics::Logger::Get().SendMessage("(FontResource) Glyph loading error.", Diagnostics::MessageType::Error);
				}
				else {
					Character char_;
					FT_Bitmap bitmap = (face)->glyph->bitmap;
					unsigned char* buffer = bitmap.buffer;
					char_.size.x = bitmap.width;
					char_.size.y = bitmap.rows;
					char_.advance.x = (face)->glyph->advance.x;
					char_.advance.y = (face)->glyph->advance.y;
					char_.bearing.x = (face)->glyph->bitmap_left;
					char_.bearing.y = (face)->glyph->bitmap_top;
					char_.charCode = charCode;
					char_.textureID = Core::ServiceLocator::Get<Windows::WindowsManager>()->GetRenderWindow()->GetGraphicsDevice()->CreateTexture(Graphics::TextureData(buffer, char_.size, 1));
					characters.insert({ charCode,char_ });
				}
			}
			charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);
		}

		Core::ServiceLocator::Get<Windows::WindowsManager>()->GetRenderWindow()->GetGraphicsDevice()->Set(Graphics::GDSettings::UNPACK_ALIGNMENT, Graphics::GDSettingsValues::FOUR_BYTES);
		return true;
	}

	Character FontResource::GetCharacter(char32_t char_)
	{
		if (characters.count(char_) == 0) return Character();
		return characters.at(char_);
	}

	Math::Vector2 FontResource::GetKerning(Character prevc, Character c)
	{
		FT_UInt prevGlyphIndex = FT_Get_Char_Index(face, prevc.charCode);
		FT_UInt glyphIndex = FT_Get_Char_Index(face, c.charCode);
		FT_Vector kerning = FT_Vector(0);
		if (prevGlyphIndex && glyphIndex) {

			FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &kerning);
		}
		return Math::Vector2(kerning.x, kerning.y);
	}
	bool FontResource::Uninit()
	{
		for (auto character : characters) //Clear old textures
		{
			if (character.second.textureID != Definitions::InvalidID)Core::ServiceLocator::Get<Windows::WindowsManager>()->GetRenderWindow()->GetGraphicsDevice()->DestroyTexture(character.second.textureID);
		}
		characters.clear();
		m_state = ResourceState::Loaded;
		return true;
	}

	bool FontResource::SetData(std::any data)
	{
		return false;
	}
	std::type_index FontResource::GetDataType() const
	{
		return typeid(void);
	}

	const std::vector<std::pair<char32_t, char32_t>> FontResource::SUPPORTED_RANGES = {
	{0x0000, 0x007F},   // ASCII
	{0x0400, 0x04FF},   
	{0x2000, 0x206F},   
	{0x20A0, 0x20CF},  
	};

} 
