#include <Services/Serialization/SerializationService.hpp>
#include <sstream>

void SerializationService::Init()
{
	if (m_isValid) return; 
	m_isValid = true;
}

void SerializationService::Shutdown()
{
	m_isValid = false;
}

std::string SerializationService::Serialize(void* data, std::type_index type)
{
	if (serializers_.count(type) == 0) return "Serializer undefined\n";		//TODO добавить логирование

	return serializers_[type].serialize(data);
}

std::string SerializationService::SerializeEntity(SceneContext& context, ECS::EntityID id)
{
	std::unordered_map<std::type_index, void*> components = context.GetEntitySpace().GetRegistry().GetAllComponents(id);

	std::stringstream text;

	auto parent = context.GetEntitySpace().GetTree().GetParent(id);

	text << "" << "ID" << " " << std::to_string(id) << "\n";
	if (parent == ECS::ROOT_ID)text << "PARENT_ID" << " " << "ROOT" << "\n";
	else text << "PARENT_ID" << " " << std::to_string(parent) << "\n";

	text << "" << "COMPONENTS" << "\n";
	text << "" << "STARTBLOCK" << "\n";

	for (auto& pair_ : components) 
	{
		text << Serialize(pair_.second, pair_.first);
	}

	text << "" << "ENDBLOCK" << "\n";

	return text.str();
}

void SerializationService::DeserializeComponents(std::vector<std::string> components_text, ECS::EntityID id, SceneContext* context)
{
	int depth = 0;
	for (size_t i = 0; i < components_text.size(); i++) 
	{
		auto line = components_text[i];
		
		

		if (line == "STARTBLOCK") depth++;
		else if (line == "ENDBLOCK") depth--;

		if (StartsWith(line, "COMPONENT_TYPE")) 
		{
			auto str_type = SplitString(line,' ')[1];
			if (string_to_type.count(str_type) == 0) continue;
			auto& type = string_to_type.at(str_type);

			std::vector<std::string> lines;

			for (size_t u = i + 1; u < components_text.size(); u++) 
			{
				auto u_line = components_text[u];
				if (u_line == "STARTBLOCK") depth++;
				else if (u_line == "ENDBLOCK") depth--;
				if (u_line == "ENDBLOCK" && depth == 1)	//У подблока PROPERTIES
				{
					i = u;
					lines.push_back(u_line);
					if (serializers_.count(type) == 0) 
					{
						break;
					}
					else 
					{
						serializers_[type].deserialize(id, lines, *context);
						break;
					}
				}
				lines.push_back(u_line);
			}
		}
	}
}
