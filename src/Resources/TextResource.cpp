#include <Resources\TextResource.hpp>
#include <fstream>
#include <sstream>

bool TextResource::Load(const std::string& path)
{
    std::ifstream in(path, std::ios::binary); // binary для корректного чтения любых символов

    if (!in.is_open())
    {
        return false;
    }

    // Читаем весь файл сразу
    std::ostringstream contentStream;
    contentStream << in.rdbuf();
    content = contentStream.str();

    return true;
}

void TextResource::Release()
{
}
