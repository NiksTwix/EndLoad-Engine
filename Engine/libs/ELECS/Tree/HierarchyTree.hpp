#pragma once
#include "ECS\Utility\Definitions.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <functional>


namespace ECS 
{
    struct HierarchyNode
    {
        std::weak_ptr<HierarchyNode> parent;
        EntityID id = ROOT_ID;
        std::vector<std::shared_ptr<HierarchyNode>> children;
    };

    class HierarchyTree
    {
    private:
        std::shared_ptr<HierarchyNode> rootNode; // Корневой узел (не принадлежит ни к какой сущности)
        std::unordered_map<EntityID, std::shared_ptr<HierarchyNode>> nodes;

        std::vector<EntityID> flatCache; // Автообновляемый кеш
        bool cacheDirty = true;

        void UpdateCache() {
            flatCache.clear();
            for (const auto& [id, node] : nodes) {
                flatCache.push_back(id);
            }
            cacheDirty = false;
        }

    public:
        HierarchyTree() {
            rootNode = std::make_shared<HierarchyNode>();
        }
        //Задаёт в качестве корневого узла конкретную сущность
        HierarchyTree(EntityID id) {
            rootNode = std::make_shared<HierarchyNode>();
            rootNode->id = id;
        }
        // Добавляет сущность как корневую. Возвращает false, если узел уже существует.
        bool AddEntityToRoot(EntityID id) {
            if (id == INVALID_ID || nodes.count(id) > 0) {
                return false;
            }

            auto node = std::make_shared<HierarchyNode>();
            node->parent = rootNode;
            node->id = id;

            nodes[id] = node;
            rootNode->children.push_back(node);
            cacheDirty = true;
            return true;
        }

        // Добавляет сущность-ребёнка к сущности-родителю. 
        // Если ребёнок уже существует, он будет перемещён к новому родителю.
        // Возвращает false, если родитель не найден или ID некорректен.
        bool AddEntityToEntity(EntityID parentId, EntityID childId) //Возвращает false, если родитель отсутсвует или id некорректен
        {
            if (parentId == INVALID_ID || childId == INVALID_ID || childId == parentId|| nodes.count(parentId) == 0) {
                return false;
            }

            auto& parentNode = nodes[parentId];

            // Если ребёнок уже есть в дереве — перемещаем его
            if (nodes.count(childId) > 0) {
                auto& childNode = nodes[childId];
                auto oldParent = childNode->parent.lock();

                if (oldParent) {
                    // Удаляем ребёнка из списка старого родителя
                    oldParent->children.erase(
                        std::remove_if(                         //Перемещает нужные элементы в конец вектора и возвращает итератор на первый из них
                            oldParent->children.begin(),
                            oldParent->children.end(),
                            [&childNode](const auto& ptr) { return ptr == childNode; }
                        ),
                        oldParent->children.end()
                    );
                }

                childNode->parent = parentNode;
                parentNode->children.push_back(childNode);
            }
            // Если ребёнка нет — создаём новый узел
            else {
                auto childNode = std::make_shared<HierarchyNode>();
                childNode->parent = parentNode;
                childNode->id = childId;

                nodes[childId] = childNode;
                parentNode->children.push_back(childNode);
            }
            cacheDirty = true;
            return true;
        }
        // Есть ли существо в иерархии
        bool HasEntity(EntityID id) const {
            return nodes.count(id) > 0;
        }

        // Удаляет сущность и всех её потомков. Возвращает false, если сущность не найдена.
        bool RemoveEntity(EntityID id) {
            if (id == INVALID_ID || nodes.count(id) == 0) {
                return false;
            }

            auto node = nodes[id];
            auto parent = node->parent.lock(); //Блокируем зацикливание

            // Удаляем из списка детей родителя
            if (parent) {
                parent->children.erase(
                    std::remove_if(
                        parent->children.begin(),
                        parent->children.end(),
                        [&node](const auto& ptr) { return ptr == node; }
                    ),
                    parent->children.end()
                );
            }

            // Рекурсивно удаляем всех детей
            std::function<void(const std::shared_ptr<HierarchyNode>&)> removeChildren;
            removeChildren = [&](const auto& node) {
                for (const auto& child : node->children) {
                    nodes.erase(child->id);
                    removeChildren(child);
                }
                };

            removeChildren(node);
            nodes.erase(id);
            cacheDirty = true;
            return true;
        }

        // Возвращает родителя сущности или InvalidEntity, если её нет или она корневая.
        EntityID GetParent(EntityID id) const {
            if (id == INVALID_ID || nodes.count(id) == 0) {
                return INVALID_ID;
            }
            auto parent = nodes.at(id)->parent.lock();
            return parent ? parent->id : INVALID_ID;
        }

        // Возвращает список детей сущности или пустой вектор, если её нет.
        const std::vector<EntityID> GetChildren(EntityID id) const {
            if (id == INVALID_ID || nodes.count(id) == 0) {
                return {};
            }
            std::vector<EntityID> childrenIds;
            for (const auto& child : nodes.at(id)->children) {
                childrenIds.push_back(child->id);
            }
            return childrenIds;
        }

        size_t GetNodesCount() const
        {
            return nodes.size();
        }

        // Быстрый доступ ко всем сущностям:
        const std::vector<EntityID>& GetAllEntities()
        {
            if (cacheDirty) UpdateCache();
            return flatCache;
        }

        const HierarchyNode* GetRoot() const
        {
            return rootNode.get();
        }
        const std::vector<EntityID> GetRootChildren() const {
            std::vector<EntityID> childrenIds;
            for (const auto& child : GetRoot()->children) {
                childrenIds.push_back(child->id);
            }
            return childrenIds;
        }
    };

}