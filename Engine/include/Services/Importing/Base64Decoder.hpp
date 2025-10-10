#pragma once
#include "SpecialHeaders/Definitions.hpp"
#include <string>
#include <algorithm>
namespace Importing 
{
    class Base64Decoder
    {
    private:
        const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

    public:
        static Base64Decoder& Get() {
            static Base64Decoder b64d;
            return b64d;
        }

        // Декодирование Base64 в байты
        std::vector<unsigned char> Decode(const std::string& encoded_string);

        // Кодирование байтов в Base64
        std::string Encode(const unsigned char* bytes_to_encode, unsigned int in_len);

        // Теперь реализуем конкретные методы:
        std::vector<float> DecodeFloatArray(const std::string& source);

        std::vector<int> DecodeIntArray(const std::string& source);

        std::vector<Definitions::uint> DecodeUIntArray(const std::string& source);

        std::string CodeFloatArray(const std::vector<float>& source);

        std::string CodeIntArray(const std::vector<int>& source);

        std::string CodeUIntArray(const std::vector<Definitions::uint>& source);
    };

}