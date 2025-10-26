#pragma once
#include <ELMath\include\MathFunctions.hpp>
#include <Components/ELComponentService.hpp>
#include <memory>
#include <mutex>
#include <ELScript\Core\Scripting\Interpreter.hpp>
#include <ELDCL/include/DCL.hpp>

namespace Scenes {
	class SceneContext;
}


namespace Components 
{
	struct ELScriptComponent
	{
		ELScript::ECID scriptID = ELScript::InvalidECID;
	};


	class ELScriptComponentService : public ELComponentService
	{
	private:
		std::mutex m_mutex;

	public:
		ELScriptComponentService();
		ELScriptComponent Create(std::filesystem::path path)	//Attachs script to component and execute it
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			ELScriptComponent script;

			script.scriptID = ELScript::Interpreter::Get().LoadScript(path);
			ELScript::Interpreter::Get().Execute(script.scriptID);
			return script;
		}

		bool Destroy(ELScriptComponent& component)
		{
			return ELScript::Interpreter::Get().DestroyScript(component.scriptID);
		}

		void InvokeProcess(Scenes::SceneContext& context, float delta);
		void InvokePhysicsProcess(Scenes::SceneContext& context, float delta);


		ELScript::Value DCL_TO_ELS(DCL::Value field);
		DCL::Value ELS_TO_DCL(ELScript::Value field);

	private:
		//Special Methods
		void AttachingAPIMethods(ELScriptComponent& script);

		ELScript::Value GetComponentELS(std::string component_name, ECS::EntityID entity, ELScript::ECID scriptID);
		ELScript::Value SetComponentELS(std::string component_name, ECS::EntityID entity, ELScript::Value data, ELScript::ECID scriptID);
	};
}
