#pragma once
#include <Resources\IResource.hpp>
#include <Systems/Graphics/GraphicsDevice/GraphicsData.hpp>
#include <MathFunctions.hpp>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H


namespace Resources 
{
    struct Character {
        Graphics::TextureID textureID = Definitions::InvalidID;
        Math::Vector2 size;
        Math::Vector2 bearing;
        Math::Vector2 advance;
        char32_t charCode;
    };

    constexpr int STANDART_FONT_SIZE = 50;

    class FontResource : public IResource
    {
    private:
        unsigned int size = 50;
        static FT_Library* GetFTLib();

        static const std::vector<std::pair<char32_t, char32_t>> SUPPORTED_RANGES;
        FT_Face face;
        std::unordered_map<char32_t, Character> characters;
    public:

        FontResource();

        bool Load(const std::string& path) override;
        bool Init() override;
        void Release() override;

        void SetSize(unsigned int size);
        unsigned int GetSize() const { return size; }
        bool GenerateBitmap();
        std::string GetType() const override { return  "Font"; }

        Character GetCharacter(char32_t char_);

        Math::Vector2 GetKerning(Character prevc, Character c);

        bool Uninit() override;


        // TODO realize 
        bool SetData(std::any data) override;

        std::type_index GetDataType() const override;

    };

}

        