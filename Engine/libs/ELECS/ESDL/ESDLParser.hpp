#pragma once
#include <ECS\Utility\Definitions.hpp>
#include <ECS\ESDL\ESDLEntityParser.hpp>
#include <ECS\ESDL\ESDLDBParser.hpp>
#include <iostream>
#include <stack>
#include <memory>
#include <algorithm>

//ECS Definition Language Parser

namespace ECS
{
	namespace ESDL 
	{
        class ESDLParser {
        public:
            struct ParseResult {
                std::vector<DataBlockTemplate> datablocks;
                std::vector<EntityTemplate> entities;
            };

            static ParseResult ParseFromFile(const std::string& file) 
            {
                return ParseFromLines(Strings::ReadLines(file));
            }

            static ParseResult ParseFromLines(const std::vector<std::string>& lines)
            {
                ParseResult result;
                if (lines.size() == 0) return result;
                
                std::vector<std::string> current_block;
                std::string current_type;
                int brace_depth = 0;

                int start_depth = 0;

                bool in_block = false;

                bool skip = false;

                for (const auto& line : lines)
                {
                    auto trimmed = Strings::Trim(line);
                    if (trimmed.empty()) continue;
                    trimmed = Strings::SplitString(trimmed, '#', 1)[0];
                    skip = false;

                    // ќбработка начала/конца блоков
                    if (Strings::StartsWith(trimmed, "DB "))
                    {
                        current_type = "DB";
                        current_block = { trimmed };
                        start_depth = brace_depth;
                        in_block = true;
                        skip = true;
                    }
                    else if (Strings::StartsWith(trimmed, "Entity "))
                    {
                        current_type = "Entity";
                        current_block = { trimmed };
                        start_depth = brace_depth;
                        in_block = true;
                        skip = true;
                    }

                    brace_depth += CountChar(trimmed, '{');
                    brace_depth -= CountChar(trimmed, '}');

                    if (in_block && !skip)
                    {
                        current_block.push_back(trimmed);

                        // ≈сли вернулись на верхний уровень - парсим блок
                        if (brace_depth == start_depth)
                        {
                            if (current_type == "DB") {
                                result.datablocks.push_back(DBParser::Parse(current_block));
                            }
                            else if (current_type == "Entity") {
                                result.entities.push_back(EntityParser::Parse(current_block));
                            }
                            in_block = false;
                        }
                    }


                    if (brace_depth < 0) throw std::runtime_error("ECS deserialization error: too many }.");
                    
                }
                return result;
            }


            //ParseResult Parse(const std::string& file)
            //{
            //    ParseResult result;
            //    auto lines = Strings::ReadLines(file);
            //
            //    std::vector<std::string> current_block;
            //    std::string current_type;
            //    int brace_depth = 0;
            //    bool in_block = false;
            //
            //    for (const auto& line : lines)
            //    {
            //        auto trimmed = Strings::Trim(line);
            //        if (trimmed.empty()) continue;
            //        trimmed = Strings::SplitString(trimmed, '#', 1)[0];
            //        
            //
            //        // ќбновл€ем глубину скобок
            //        brace_depth += CountChar(trimmed, '{');
            //        brace_depth -= CountChar(trimmed, '}');
            //
            //        // ќбработка начала/конца блоков
            //        if (Strings::StartsWith(trimmed, "DB "))
            //        {
            //            current_type = "DB";
            //            current_block = { trimmed };
            //            in_block = true;
            //        }
            //        else if (Strings::StartsWith(trimmed, "Entity "))
            //        {
            //            current_type = "Entity";
            //            current_block = { trimmed };
            //            in_block = true;
            //        }
            //        else if (in_block)
            //        {
            //            current_block.push_back(trimmed);
            //
            //            // ≈сли вернулись на верхний уровень - парсим блок
            //            if (brace_depth == 0)
            //            {
            //                if (current_type == "DB") {
            //                    result.datablocks.push_back(DBParser::Parse(current_block));
            //                }
            //                else if (current_type == "Entity") {
            //                    result.entities.push_back(EntityParser::Parse(current_block));
            //                }
            //                in_block = false;
            //            }
            //        }
            //    }
            //    return result;
            //}

        private:
            static int CountChar(const std::string& str, char ch)
            {
                return std::count(str.begin(), str.end(), ch);
            }
        };
	}
	
}