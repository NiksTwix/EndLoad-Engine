#pragma once
#include "..\Utility\Definitions.hpp"
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
        std::shared_ptr<HierarchyNode> rootNode; // �������� ���� (�� ����������� �� � ����� ��������)
        std::unordered_map<EntityID, std::shared_ptr<HierarchyNode>> nodes;

        std::vector<EntityID> flatCache; // ��������������� ���
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
        //����� � �������� ��������� ���� ���������� ��������
        HierarchyTree(EntityID id) {
            rootNode = std::make_shared<HierarchyNode>();
            rootNode->id = id;
        }
        // ��������� �������� ��� ��������. ���������� false, ���� ���� ��� ����������.
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

        // ��������� ��������-������ � ��������-��������. 
        // ���� ������ ��� ����������, �� ����� ��������� � ������ ��������.
        // ���������� false, ���� �������� �� ������ ��� ID �����������.
        bool AddEntityToEntity(EntityID parentId, EntityID childId) //���������� false, ���� �������� ���������� ��� id �����������
        {
            if (parentId == INVALID_ID || childId == INVALID_ID || childId == parentId|| nodes.count(parentId) == 0) {
                return false;
            }

            auto& parentNode = nodes[parentId];

            // ���� ������ ��� ���� � ������ � ���������� ���
            if (nodes.count(childId) > 0) {
                auto& childNode = nodes[childId];
                auto oldParent = childNode->parent.lock();

                if (oldParent) {
                    // ������� ������ �� ������ ������� ��������
                    oldParent->children.erase(
                        std::remove_if(                         //���������� ������ �������� � ����� ������� � ���������� �������� �� ������ �� ���
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
            // ���� ������ ��� � ������ ����� ����
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
        // ���� �� �������� � ��������
        bool HasEntity(EntityID id) const {
            return nodes.count(id) > 0;
        }

        // ������� �������� � ���� � ��������. ���������� false, ���� �������� �� �������.
        bool RemoveEntity(EntityID id) {
            if (id == INVALID_ID || nodes.count(id) == 0) {
                return false;
            }

            auto node = nodes[id];
            auto parent = node->parent.lock(); //��������� ������������

            // ������� �� ������ ����� ��������
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

            // ���������� ������� ���� �����
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

        // ���������� �������� �������� ��� InvalidEntity, ���� � ��� ��� ��� ��������.
        EntityID GetParent(EntityID id) const {
            if (id == INVALID_ID || nodes.count(id) == 0) {
                return INVALID_ID;
            }
            auto parent = nodes.at(id)->parent.lock();
            return parent ? parent->id : INVALID_ID;
        }

        // ���������� ������ ����� �������� ��� ������ ������, ���� � ���.
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

        // ������� ������ �� ���� ���������:
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