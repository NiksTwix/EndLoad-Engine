#include <Components\UI\TextRenderComponent.hpp>

void TextRenderService::Init()
{
}

void TextRenderService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}

void TextRenderService::Shutdown()
{
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> TextRenderService::GetComponentFields(void* component)
{
    return std::unordered_map<std::string, ECS::ESDL::ESDLType>();
}

void TextRenderService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
}

Math::Vector2 TextRenderService::CalculateTextSize(const TextRenderComponent& text)
{
    float width = 0.0f;
    float height = 0.0f;

    for (char c : text.Text) {
        Character ch = text.Font->GetCharacter(c);
        width += ((int)ch.advance.x >> 6) * text.RenderScale.x;
        height = std::max(height, ch.size.y * text.RenderScale.y);
    }

    return Math::Vector2(width, height);
}

Math::Vector2 TextRenderService::GetAlignmentOffset(const TextRenderComponent& text, const Math::Vector2& uiSize, const Math::Vector2& textSize)
{
    Math::Vector2 offset(0);
    float baseOffset = GetTextBaseOffset(text);

    // Горизонтальное выравнивание
    switch (text.XAlignment) {
    case StringAlignment::Near:   offset.x = -uiSize.x / 2  + text.XOffset; break;
    case StringAlignment::Middle: offset.x = -textSize.x / 2 + text.XOffset; break;
    case StringAlignment::Far:    offset.x = uiSize.x / 2 - textSize.x - text.XOffset; break;
    }

    // Вертикальное выравнивание
    switch (text.YAlignment) {
    case StringAlignment::Near:   offset.y = -uiSize.y / 2 + textSize.y + text.YOffset; break;
    case StringAlignment::Middle: offset.y = textSize.y/2 + text.YOffset; break;
    case StringAlignment::Far:    offset.y = uiSize.y/2 - baseOffset - text.YOffset; break;
    }
    return offset;
}

float TextRenderService::GetTextBaseOffset(const TextRenderComponent& text)
{
    float maxUnderBaseline = 0.0f;

    // Ищем символы с максимальным "хвостом" (p, q, g и т.д.)
    for (char c : text.Text) {
        const auto& ch = text.Font->GetCharacter(c);
        float underBaseline = ch.size.y - ch.bearing.y; // Часть символа под базовой линией
        if (underBaseline > maxUnderBaseline) {
            maxUnderBaseline = underBaseline;
        }
    }

    return maxUnderBaseline * text.RenderScale.y;
}
