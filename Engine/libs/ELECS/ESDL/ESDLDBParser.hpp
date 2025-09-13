#pragma once
#include <ECS\Utility\Definitions.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>


namespace ECS
{
    namespace ESDL
    {
        class DBParser
        {
        public:
            static DataBlockTemplate Parse(const std::vector<std::string>& block)
            {
                DataBlockTemplate db;
                int brace_depth = 0;
                std::string current_key;

                for (const auto& line : block)
                {
                    auto trimmed = Strings::Trim(line);
                    if (trimmed.empty()) continue;

                    // Обработка вложенности
                    brace_depth += CountChar(trimmed, '{');

                    if (brace_depth == 0) continue; // Пропускаем строки вне блока

                    // Конец блока
                    if (trimmed == "}" || Strings::EndsWith(trimmed, "}"))
                    {
                        if (--brace_depth == 0) break;
                        continue;
                    }

                    // Парсинг свойств
                    if (brace_depth == 1)
                    {
                        auto colon_pos = trimmed.find(':');
                        if (colon_pos != std::string::npos)
                        {
                            current_key = Strings::Trim(trimmed.substr(0, colon_pos));
                            std::string value = Strings::Trim(trimmed.substr(colon_pos + 1));
                            db.properties[current_key] = ValueParser::Parse(value);
                        }
                    }
                }
                return db;
            }

        private:
            

            static int CountChar(const std::string& str, char ch)
            {
                return std::count(str.begin(), str.end(), ch);
            }
        };
    }

}