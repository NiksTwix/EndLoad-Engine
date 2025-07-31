#include <Services/Utility\Functions.hpp>
#include <string_view>
#include <iostream>

std::vector<std::string> SplitString(const std::string& str, char delimiter, int limit) {
    std::vector<std::string> tokens;
    try
    {
        if (str.empty()) return tokens;
        if (limit == 0) return tokens;

        size_t start = 0;
        size_t end = str.find(delimiter);
        size_t count = 0;

        

        while (end != std::string::npos)
        {
            // Безопасное извлечение подстроки
            if (start < str.length()) {
                tokens.push_back(str.substr(start, end - start));
                if (limit > 0 && ++count >= static_cast<size_t>(limit)) {
                    tokens.push_back(str.substr(end+1));
                    return tokens;
                }
            }
            start = end + 1;
            end = str.find(delimiter, start);
        }

        // Добавляем остаток строки (последний токен)
        if (start < str.length()) {
            tokens.push_back(str.substr(start));
        }
    }
    catch (std::exception e) {
        std::cout << "SplitString error:" << e.what() << "\n";
        std::cout << "String is " << str << "\n";
    }

    return tokens;
}

bool StartsWith(const std::string& source, const std::string& start)
{
    size_t start_length = start.size();
    if (source.size() < start_length) return false;


    for (size_t i = 0; i < start_length; i++)
    {
        if (source[i] != start[i])return false;
    }

    return true;
}

bool EndsWith(const std::string& source, const std::string& end) {
    if (end.empty()) return true;  // Любая строка заканчивается пустой подстрокой
    if (source.size() < end.size()) return false;

    return source.compare(source.size() - end.size(), end.size(), end) == 0;
}

std::vector<std::string> SplitToLines(const std::string& str)
{
    std::vector<std::string> lines;
    std::istringstream iss(str);
    std::string line;

    while (std::getline(iss, line)) {  // getline по умолчанию удаляет \r
        if (line.size() == 0) continue;
        if (line[line.size() - 1] == '\r')line.pop_back();
        lines.push_back(line);
    }

    return lines;
}

std::string EraseString(const std::string& str, const std::string& sub_str) {
    // 1. Проверка тривиальных случаев
    if (sub_str.empty() || str.empty() || sub_str.length() > str.length()) {
        return str;
    }

    std::string result;
    result.reserve(str.length()); // Оптимизация памяти

    size_t last_pos = 0;
    size_t pos = str.find(sub_str);

    // 2. Основной цикл удаления
    while (pos != std::string::npos) {
        // Добавляем часть перед подстрокой (если есть)
        if (pos > last_pos) {
            result.append(str, last_pos, pos - last_pos);
        }

        last_pos = pos + sub_str.length();
        pos = str.find(sub_str, last_pos);
    }

    // 3. Добавляем остаток строки
    if (last_pos < str.length()) {
        result.append(str, last_pos, str.length() - last_pos);
    }

    // 4. Удаляем возможные пробелы в начале/конце
    auto trim = [](std::string& s) {
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);
        };

    trim(result);
    return result;
}


bool IsEqualAround(float first, float second, float precision)
{
    return std::abs(first - second) <= precision;
}




