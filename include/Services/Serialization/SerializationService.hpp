#pragma once
#include <functional>
#include <fstream>
#include <typeindex>
#include <string>
#include <Services\Scene\SceneContext.hpp>
#include <Core\IServices.hpp>
#include <Services\Utility\Functions.hpp>


class SerializationService : public IHelperService 
{
public:
	using SerializeFunc = std::function<std::string(void*)>;
	using DeserializeFunc = std::function<void(ECS::EntityID, std::vector<std::string>&, SceneContext&)>;	//Автоматический добавляет компонент к существу
	SerializationService() = default;
	void Init() override;
	void Shutdown() override;

	template<typename T>
	void Register(SerializeFunc serialize,DeserializeFunc deserialize)
	{
		Serializer ser;
		ser.serialize = serialize;
		ser.deserialize = deserialize;
		std::type_index t = typeid(T);

		//serializers_[t] = ser;
		serializers_.insert({ t,ser });
		string_to_type.insert({ SplitString(std::string(typeid(T).name()),' ')[1],typeid(T) });
	}

	std::string Serialize(void* data, std::type_index type);

	std::string SerializeEntity(SceneContext& context, ECS::EntityID id); //Сериализует существо в формате ELScene




	void DeserializeComponents(std::vector<std::string> components_text, ECS::EntityID id, SceneContext* context);		//WARNING нет определения

private:
	struct Serializer {
		SerializeFunc serialize;
		DeserializeFunc deserialize;
	};
	std::unordered_map<std::type_index, Serializer> serializers_;
	std::unordered_map<std::string, std::type_index>  string_to_type; // Превращения строки (имени типа в type_index)
};