#include <Resources\TextResource.hpp>
#include <fstream>
#include <sstream>

bool TextResource::Load(const std::string& path)
{
    std::ifstream in(path, std::ios::binary); // binary ��� ����������� ������ ����� ��������

    if (!in.is_open())
    {
        return false;
    }

    // ������ ���� ���� �����
    std::ostringstream contentStream;
    contentStream << in.rdbuf();
    content = contentStream.str();

    return true;
}

void TextResource::Release()
{
}
