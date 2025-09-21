#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "..\Utility\Definitions.hpp"


namespace ECS 
{
	template<typename T>
	class SparseSet
	{
	private:
		std::vector<T> data;			// ����������
		std::vector<EntityID> dense;	//������� ������ �������
		std::vector<EntityID> sparse;	//���������� ������ � ��������� �� �������� dense;
	public:

		bool Has(EntityID entity) const
		{
			return entity < sparse.size() && sparse[entity] != INVALID_ID;
		}

		void Add(EntityID entity, const T& component) 
		{
			if (Has(entity)) {
				throw std::runtime_error("ECS: Entity " + std::to_string(entity) + " already has this component");
			}
			if (entity >= sparse.size())
			{
				sparse.resize(entity + 1, INVALID_ID);
			}
			sparse[entity] = dense.size();
			dense.push_back(entity);
			data.push_back(component);
			/**
			* ��������� ��������� � ��������.
			*
			* WARNING ������� ����� ����������� ����������.
			* ��� ������� �������� (������, �������) �������������:
			* 1. ���� ������������ shared_ptr-������ (AddShared())
			* 2. ���� ���� ��������� �������� ����� �������
			*
			* @example
			* // ������� 1 - ����������� �����:
			* Camera cam = service->CreateCamera();
			* registry.Add<Camera>(entity, cam); // �����
			*
			* // ������� 2 - ����� ������:
			* auto cam = service->CreateCameraShared();
			* registry.AddShared<Camera>(entity, cam);
			*/
			
		}

		void Replace(EntityID entity, const T& component)	//������ ������ ��������� �� �����
		{
			if (!Has(entity)) {
				Add(entity, component);
				return;
			}

			data[sparse[entity]] = T(component);

		}

		void Remove(EntityID entity) 
		{
			if (!Has(entity)) return;
			
			size_t index = sparse[entity];
			EntityID lastEntity = dense.back();

			dense[index] = lastEntity;  // ���������� ��������� ��������
			data[index] = std::move(data.back());  // � � ���������

			sparse[lastEntity] = index;  // ��������� ������ ��� ������������ ��������

			dense.pop_back();
			data.pop_back();

			sparse[entity] = INVALID_ID;
		}

		const T& Get(EntityID entity) const //������
		{
			if (!Has(entity)) {
				throw std::runtime_error("ECS: Entity " + std::to_string(entity) + " hasn't component");
			}
			return data[sparse[entity]];
		}
		T& Get(EntityID entity) {  // ��� �����������
			return const_cast<T&>(const_cast<const SparseSet*>(this)->Get(entity));
		}

		void Reserve(size_t capacity) {
			dense.reserve(capacity);
			data.reserve(capacity);
		}

		auto Begin() const { return data.begin(); }
		auto End() const { return data.end(); }

		template<typename... Args>
		T& Emplace(EntityID entity, Args&&... args) {
			if (entity >= sparse.size()) {
				sparse.resize(entity + 1, INVALID_ID);
			}

			sparse[entity] = dense.size();
			dense.push_back(entity);
			data.emplace_back(std::forward<Args>(args)...); // ����������� �� �����

			return data.back();
		}

		std::vector<EntityID> GetAllEntities() const 
		{
			return dense;
		}


		void Clear() 
		{
			data.clear();
			dense.clear();
			sparse.clear();
		}

	};
}


