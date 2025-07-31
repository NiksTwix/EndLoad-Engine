#include <Resources\FontResource.hpp>
#include <Core\ServiceLocator.hpp>
#include <Core\Logger.hpp>
#include <Systems\Render\Render.hpp>

FontResource::FontResource()
{
}

bool FontResource::Load(const std::string& path)
{
    auto ft = *GetFTLib();

    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        ServiceLocator::Get<Logger>()->Log("(FontResource): font loading error.", Logger::Level::Error);
        return false;
    }

    SetSize(50);

    return true;
}

void FontResource::Release()
{
    FT_Done_Face(face);
}

void FontResource::SetSize(unsigned int size)
{
    if (!face) return;

    FT_Set_Pixel_Sizes(face, 0, size);
    this->size = size;
    GenerateBitmap();
}

Character FontResource::GetCharacter(char32_t char_)
{
    if (characters.count(char_) == 0) return Character();
    return characters.at(char_);
}

void FontResource::GenerateBitmap()
{
    if (!face) return;
    FT_UInt glyphIndex;
    char32_t charCode = FT_Get_First_Char(face, &glyphIndex);

    ServiceLocator::Get<Render>()->GetContext()->Set(RCSettings::UNPACK_ALIGNMENT, RCSettingsValues::ONE_BYTE);

    while (glyphIndex != 0) {
        // Проверяем, что символ в нужном диапазоне
        bool isSupported = false;
        for (const auto& range : SUPPORTED_RANGES) {
            if (charCode >= range.first && charCode <= range.second) {
                isSupported = true;
                break;
            }
        }

        if (isSupported) {
            if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER)) {
                ServiceLocator::Get<Logger>()->Log("(FontResource): glyph loading error.", Logger::Level::Error);
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
                char_.textureID = ServiceLocator::Get<Render>()->GetContext()->CreateTexture(buffer, char_.size,1);
                characters.insert({ charCode,char_ });
            }
        }
        charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);
    }

    ServiceLocator::Get<Render>()->GetContext()->Set(RCSettings::UNPACK_ALIGNMENT, RCSettingsValues::FOUR_BYTES);
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


FT_Library* FontResource::GetFTLib()
{
    static FT_Library ftLib = nullptr;
    static std::once_flag initFlag;

    // Лямбда для освобождения ресурса
    auto freeFTLib = []() {
        if (ftLib) {
            FT_Done_FreeType(ftLib);
            ftLib = nullptr;
        }
        };

    std::call_once(initFlag, [&]() {
        if (FT_Init_FreeType(&ftLib)) {
            ServiceLocator::Get<Logger>()->Log("(FontResource): FT library isnt inited.", Logger::Level::Error);
            ftLib = nullptr;
        }
        else {
            std::atexit(freeFTLib);  // Освобождаем при выходе
        }
        });

    return &ftLib;
}

const std::vector<std::pair<char32_t, char32_t>> FontResource::SUPPORTED_RANGES = {
    {0x0000, 0x007F},   // ASCII
    {0x0400, 0x04FF},   // Кириллица
    {0x2000, 0x206F},   // Пунктуация
    {0x20A0, 0x20CF},   // Валютные символы
    // Добавьте другие нужные диапазоны...
};
