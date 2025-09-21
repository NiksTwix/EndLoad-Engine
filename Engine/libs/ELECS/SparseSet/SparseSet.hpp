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
		std::vector<T> data;			// компоненты
		std::vector<EntityID> dense;	//Плотный массив существ
		std::vector<EntityID> sparse;	//Разрежённый массив с индексами на существо dense;
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
			* Добавляет компонент к сущности.
			*
			* WARNING Создает КОПИЮ переданного компонента.
			* Для сложных объектов (камеры, ресурсы) рекомендуется:
			* 1. Либо использовать shared_ptr-версию (AddShared())
			* 2. Либо явно управлять временем жизни объекта
			*
			* @example
			* // Вариант 1 - независимая копия:
			* Camera cam = service->CreateCamera();
			* registry.Add<Camera>(entity, cam); // Копия
			*
			* // Вариант 2 - общий ресурс:
			* auto cam = service->CreateCameraShared();
			* registry.AddShared<Camera>(entity, cam);
			*/
			
		}

		void Replace(EntityID entity, const T& component)	//Меняет старый компонент на новый
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

			dense[index] = lastEntity;  // Перемещаем последнюю сущность
			data[index] = std::move(data.back());  // И её компонент

			sparse[lastEntity] = index;  // Обновляем индекс для перемещённой сущности

			dense.pop_back();
			data.pop_back();

			sparse[entity] = INVALID_ID;
		}

		const T& Get(EntityID entity) const //Чтение
		{
			if (!Has(entity)) {
				throw std::runtime_error("ECS: Entity " + std::to_string(entity) + " hasn't component");
			}
			return data[sparse[entity]];
		}
		T& Get(EntityID entity) {  // Для модификации
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
			data.emplace_back(std::forward<Args>(args)...); // Конструкция на месте

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


