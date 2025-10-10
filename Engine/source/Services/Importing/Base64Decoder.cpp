#include <Services\Importing\Base64Decoder.hpp>


namespace Importing 
{
    std::vector<unsigned char> Base64Decoder::Decode(const std::string& encoded_string)
    {
        int in_len = encoded_string.size();
        int i = 0, j = 0, in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> ret;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; i < 3; i++)
                    ret.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; j < i - 1; j++)
                ret.push_back(char_array_3[j]);
        }

        return ret;
    }
    std::string Base64Decoder::Encode(const unsigned char* bytes_to_encode, unsigned int in_len)
    {
        std::string ret;
        int i = 0, j = 0;
        unsigned char char_array_3[3], char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0'; //«аполн€ем пустыми данными, если что-то не сошлось

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; j < i + 1; j++)
                ret += base64_chars[char_array_4[j]];

            while (i++ < 3)
                ret += '=';
        }

        return ret;
    }
    std::vector<float> Base64Decoder::DecodeFloatArray(const std::string& source)
    {
        std::vector<unsigned char> bytes = Decode(source);
        std::vector<float> result;

        // ѕреобразуем байты обратно в float
        for (size_t i = 0; i + sizeof(float) <= bytes.size(); i += sizeof(float)) {
            float value;
            std::memcpy(&value, &bytes[i], sizeof(float));
            result.push_back(value);
        }

        return result;
    }
    std::vector<int> Base64Decoder::DecodeIntArray(const std::string& source)
    {
        std::vector<unsigned char> bytes = Decode(source);
        std::vector<int> result;

        for (size_t i = 0; i + sizeof(int) <= bytes.size(); i += sizeof(int)) {
            int value;
            std::memcpy(&value, &bytes[i], sizeof(int));
            result.push_back(value);
        }

        return result;
    }
    std::vector<Definitions::uint> Base64Decoder::DecodeUIntArray(const std::string& source)
    {
        std::vector<unsigned char> bytes = Decode(source);
        std::vector<Definitions::uint> result;

        for (size_t i = 0; i + sizeof(Definitions::uint) <= bytes.size(); i += sizeof(Definitions::uint)) {
            Definitions::uint value;
            std::memcpy(&value, &bytes[i], sizeof(Definitions::uint));
            result.push_back(value);
        }

        return result;
    }
    std::string Base64Decoder::CodeFloatArray(const std::vector<float>& source)
    {
        std::vector<unsigned char> bytes;

        for (const float& value : source) {
            const unsigned char* value_bytes = reinterpret_cast<const unsigned char*>(&value);
            bytes.insert(bytes.end(), value_bytes, value_bytes + sizeof(float));
        }

        return Encode(bytes.data(), bytes.size());
    }
    std::string Base64Decoder::CodeIntArray(const std::vector<int>& source)
    {
        std::vector<unsigned char> bytes;

        for (const int& value : source) {
            const unsigned char* value_bytes = reinterpret_cast<const unsigned char*>(&value);
            bytes.insert(bytes.end(), value_bytes, value_bytes + sizeof(int));
        }

        return Encode(bytes.data(), bytes.size());
    }
    std::string Base64Decoder::CodeUIntArray(const std::vector<Definitions::uint>& source)
    {
        std::vector<unsigned char> bytes;

        for (const Definitions::uint& value : source) {
            const unsigned char* value_bytes = reinterpret_cast<const unsigned char*>(&value);
            bytes.insert(bytes.end(), value_bytes, value_bytes + sizeof(Definitions::uint));
        }

        return Encode(bytes.data(), bytes.size());
    }
}