#pragma once
#include <Resources\IResource.hpp>
#include <Math\MathFunctions.hpp>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H


struct Character {
    unsigned int textureID;
    Math::Vector2 size;
    Math::Vector2 bearing;
    Math::Vector2 advance;
    char32_t charCode;
};

class FontResource : public IResource
{
private:

    unsigned int size = 50;

    static FT_Library* GetFTLib();

    static const std::vector<std::pair<char32_t, char32_t>> SUPPORTED_RANGES;

    void GenerateBitmap();

    FT_Face face;



    std::unordered_map<char32_t, Character> characters;
public:

    FontResource();

    bool Load(const std::string& path) override;

    void Release() override;

    void SetSize(unsigned int size);
    unsigned int GetSize() { return size; }

    std::string GetType() const override { return  "Font"; }   

    Character GetCharacter(char32_t char_);

    Math::Vector2 GetKerning(Character prevc, Character c);
};
                   