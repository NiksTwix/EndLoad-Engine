#pragma once
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <memory>
#include <sys/types.h>
#include <algorithm>

namespace ECS
{
	namespace ESDL
	{
		class Strings 
		{
		public:
			static std::vector<std::string> ReadLines(const std::string& filename) {
                std::vector<std::string> lines;
                std::ifstream file(filename);

                if (!file.is_open()) {
                    throw std::runtime_error("ESDL error: Failed to open file: " + filename);
                }

                std::string line;
                while (std::getline(file, line)) {
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    lines.push_back(line);
                }

                return lines;
            }
            static bool StartsWith(const std::string& source, const std::string& start)
            {
                size_t start_length = start.size();
                if (source.size() < start_length) return false;


                for (size_t i = 0; i < start_length; i++)
                {
                    if (source[i] != start[i])return false;
                }

                return true;
            }

            static bool EndsWith(const std::string& source, const std::string& end) {
                if (end.empty()) return true;  // Любая строка заканчивается пустой подстрокой
                if (source.size() < end.size()) return false;

                return source.compare(source.size() - end.size(), end.size(), end) == 0;
            }
            static std::string Trim(const std::string& str) {
                size_t first = str.find_first_not_of(" \t");
                if (std::string::npos == first) return "";
                size_t last = str.find_last_not_of(" \t");
                return str.substr(first, (last - first + 1));
            }

            static std::vector<std::string> Tokenize(const std::string& str) {
                std::vector<std::string> tokens;
                std::string current;
                bool in_quotes = false;

                for (char c : str) {
                    if (c == '"') {
                        in_quotes = !in_quotes;
                        current += c;
                    }
                    else if ((std::isspace(c) || c == '{' || c == '}') && !in_quotes) {
                        if (!current.empty()) {
                            tokens.push_back(current);
                            current.clear();
                        }
                        if (c == '{' || c == '}') {
                            tokens.push_back(std::string(1, c));
                        }
                    }
                    else {
                        current += c;
                    }
                }

                if (!current.empty()) {
                    tokens.push_back(current);
                }

                return tokens;
            }


            static bool IsNumber(const std::string& s) {
                if (s.empty()) return false;

                bool hasDecimalPoint = false;
                size_t start = 0;

                // Проверка на знак в начале
                if (s[0] == '+' || s[0] == '-') {
                    start = 1;
                    if (s.size() == 1) return false; // Только знак - не число
                }

                for (size_t i = start; i < s.size(); ++i) {
                    char c = s[i];

                    if (c == '.') {
                        if (hasDecimalPoint) return false; // Две точки
                        hasDecimalPoint = true;
                    }
                    else if (!std::isdigit(static_cast<unsigned char>(c))) {
                        return false;
                    }
                }
                // Проверка, что строка не заканчивается точкой
                return !(hasDecimalPoint && s.back() == '.');
            }


            static std::vector<std::string> SplitString(const std::string& str, char delimiter, int limit = -1) {
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
                                tokens.push_back(str.substr(end + 1));
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
            static std::string EraseString(const std::string& str, const std::string& sub_str) {
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

            static std::string GetIndentByDepth(int depth)
            {
                if (depth == 0) return "";
                std::string result;
                for (int i = 1; i <= depth; i++)
                {
                    result += "\t";
                }
                return result;
            }
		};

	}
     
}

