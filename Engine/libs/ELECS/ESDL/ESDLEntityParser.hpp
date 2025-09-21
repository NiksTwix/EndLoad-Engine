#pragma once
#include "..\Utility\Definitions.hpp"
#include "ESDLStructs.hpp"
#include "ESDLStrings.hpp"
#include "ESDLValueParser.hpp"

namespace ECS
{
    namespace ESDL
    {
        class EntityParser
        {
        public:
            static EntityTemplate Parse(const std::vector<std::string>& block)
            {
                EntityTemplate entity;
                int brace_depth = 0;
                ComponentTemplate* current_component = nullptr;

                for (const auto& line : block)
                {
                    auto tokens = Strings::Tokenize(line);
                    if (tokens.empty()) continue;

                    for (size_t i = 0; i < tokens.size(); ++i)
                    {
                        const auto& token = tokens[i];

                        if (token == "Entity" && i + 1 < tokens.size())
                        {
                            ParseEntityID(tokens[i + 1], entity);
                            ++i; // Skip ID
                        }
                        else if (token == "{")
                        {
                            ++brace_depth;
                        }
                        else if (token == "}")
                        {
                            if (--brace_depth == 1) {
                                current_component = nullptr;
                            }
                        }
                        else if (brace_depth == 1 && !current_component)
                        {
                            // New component
                            entity.components.emplace_back();
                            current_component = &entity.components.back();
                            current_component->type = token;
                        }
                        else if (current_component && ((i + 1 < tokens.size() && tokens[i + 1] == ":") || tokens[i].back() == ':'))
                        {
                            // Component property

                            if (tokens[i].back() == ':')
                            {
                                tokens[i].pop_back();
                                current_component->properties[tokens[i]] =
                                    ValueParser::Parse(JoinTokens(tokens, i + 1));
                                break;
                            }
                            current_component->properties[tokens[i]] =
                                ValueParser::Parse(JoinTokens(tokens, i + 2));
                            break;
                        }
                    }
                }
                return entity;
            }

        private:
            static void ParseEntityID(const std::string& id_str, EntityTemplate& entity)
            {
                auto parts = Strings::SplitString(id_str, ':');
                if (parts.empty()) throw std::runtime_error("ESDL error: Invalid Entity ID.");

                entity.id = std::stoi(parts[0]);
                entity.parentID = (parts.size() > 1) ? std::stoi(parts[1]) : INVALID_ID;
            }

            static std::string JoinTokens(const std::vector<std::string>& tokens,
                size_t start_idx)
            {
                std::string result;
                for (size_t i = start_idx; i < tokens.size(); ++i) {
                    if (!result.empty()) result += " ";
                    result += tokens[i];
                }
                return result;
            }
        };
    }

}