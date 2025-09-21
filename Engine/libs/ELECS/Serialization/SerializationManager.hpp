#pragma once
#include "..\ESDL\ESDLParser.hpp"
#include "..\Main\EntitySpace.hpp"



namespace ECS 
{

    class SerializationManager {
    public:
        // �������������� �� ������
        static void Deserialize(EntitySpace& e_space, const std::string& text) {
            if (!e_space.GetAllEntities().empty()) {
                e_space.ClearAllEntities();
            }
            DeserializeFromLines(e_space, ESDL::Strings::SplitString(text, '\n'));
        }

        // �������������� �� �����
        static void DeserializeFromLines(EntitySpace& e_space, const std::vector<std::string>& lines) {
           auto parseResult = ESDL::ESDLParser::ParseFromLines(lines);
            ComponentServiceLocator& csl = e_space.GetServiceLocator();
            std::unordered_map<EntityID, EntityID> childToParent;

            e_space.UpdateAllServices();

            // �������� ������� �����������
            std::unordered_map<std::string, IComponentService*> componentServices;
            for (auto* service : csl.GetAllServices()) {
                if (!service) continue; // ������ �� nullptr
                componentServices[service->GetStringComponentType()] = service;
            }

            // ������ �������� � ����������
            for (const auto& entityDesc : parseResult.entities) {
                EntityID newEntity = e_space.CreateEntityWithID(entityDesc.id);
                if (entityDesc.parentID != INVALID_ID) {
                    childToParent[entityDesc.id] = entityDesc.parentID;
                }

                for (const auto& component : entityDesc.components) {
                    auto it = componentServices.find(component.type);
                    if (it == componentServices.end()) {
                        throw std::runtime_error(
                            "ECS deserialization error: component type '" +
                            component.type + "' not registered for entity " +
                            std::to_string(entityDesc.id)
                        );
                    }
                    it->second->AddComponentByFields(newEntity, component.properties);
                }
            }

            // ��������������� ��������
            for (const auto& [child, parent] : childToParent) {
                e_space.AddEntity(child, parent);
            }
        }

        // ������������ � ������
        static std::vector<std::string> SerializeToLines(EntitySpace& e_space) {
            std::vector<std::string> result;
            const auto& entities = e_space.GetAllEntities();

            for (EntityID entity : entities) {
                EntityID parent = e_space.GetTree().GetParent(entity);

                // ��������� �������� (��������, "Entity 1:2")
                std::string entityHeader = "Entity " + std::to_string(entity);
                if (parent != ROOT_ID) {
                    entityHeader += ":" + std::to_string(parent);
                }
                result.push_back(entityHeader);
                result.push_back("{");

                // ������������ �����������
                for (auto* service : e_space.GetServiceLocator().GetAllServices()) {
                    if (!service || !e_space.GetRegistry().HasByTypeIndex(service->GetComponentType(), entity)) {
                        continue;
                    }

                    const std::string componentType = service->GetStringComponentType();
                    result.push_back(Indent(1) + componentType);
                    result.push_back(Indent(1) + "{");

                    void* componentData = e_space.GetRegistry().GetByTypeIndex(service->GetComponentType(), entity);
                    for (const auto& [fieldName, fieldValue] : service->GetComponentFields(componentData)) {
                        result.push_back(Indent(2) + fieldName + " : " + ESDL::ValueParser::ToString(fieldValue));
                    }

                    result.push_back(Indent(1) + "}");
                }

                result.push_back("}");
                result.push_back(""); // ������ ������ ��� ����������
            }

            return result;
        }

    private:
        static std::string Indent(int depth) {
            return std::string(depth * 4, ' '); // 4 ������� �� �������
        }
    };
}