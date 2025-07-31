#pragma once
#include <Math\MathFunctions.hpp>
#include <Core/IServices.hpp>
#include <ECS\ECS.hpp>
#include <memory>
#include <mutex>




class ObjectScript 
{
protected:
	std::string Name = "ObjectScript";
	ECS::EntityID entityID;
public:

	ObjectScript(ECS::EntityID id) { entityID = id; }

	virtual void Ready() {};				//Вызывается при добавлении объекта в дерево сцены
	virtual void Process(float delta) {};				//Вызывается в конце каждого графического кадра
	virtual void PhysicsProcess(float delta) {};		//Вызывается в конце каждого физического кадра
	virtual void Destroyed() {};

	std::string GetName() { return Name; }
	void SetName(std::string name) { Name = name; }
};

class SceneContext;


struct ScriptComponent 
{
	std::shared_ptr<ObjectScript> script;
};


class ScriptService : public ECS::IComponentService
{
private:
	std::mutex m_mutex;

public:
	ScriptService();
	template<typename TScript, typename... Args>
	ScriptComponent Create(ECS::EntityID id, Args&&... args)	//Прикрепляет скрипт к объекту. Вызывает метод Ready!
	{
		static_assert(std::is_base_of_v<ObjectScript, TScript>, "TScript must inherit from ObjectScript");

		std::lock_guard<std::mutex> lock(m_mutex);

		ScriptComponent script;

		script.script = std::make_shared<TScript>(id, std::forward<Args>(args)...);

		script.script->Ready();

		return script;
	}

	void InvokeProcess(SceneContext& context, float delta);
	void InvokePhysicsProcess(SceneContext& context, float delta);

	void RemoveScript(ECS::EntityID id, SceneContext& context);
	// Унаследовано через IComponentService
	void Init() override;
	void Update(ECS::EntitySpace* eSpace) override;
	void Shutdown() override;
	std::unordered_map<std::string, ECS::ESDL::ESDLType> GetComponentFields(void* component) override;
	void AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> field) override;
	//Открепляет скрипт. Вызывает у скрипта Destroyed!
};