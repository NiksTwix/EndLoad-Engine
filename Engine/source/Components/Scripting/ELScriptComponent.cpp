#include <Components\Scripting\ELScriptComponent.hpp>
#include <Services\Scenes\SceneContext.hpp>
//#include <Services\Events\EventManager.hpp>
#include <Core\ServiceLocator.hpp>
#include <Components\Components.hpp>
#include <Systems\Input\InputSystem.hpp>
#include <Components/Basic/TransformComponent.hpp>
namespace Components
{
    ELScriptComponentService::ELScriptComponentService()
    {
        m_stringType = "ELScript";
        m_ComponentType = typeid(ELScriptComponent);

        ELScript::FunctionTable::Register("get_component", 2, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args)//component_name, entity
            {
                if (args.size() < 2) return ELScript::Value();

                return GetComponentELS(args[0].strVal, (int)args[1].numberVal, id);
            });
        ELScript::FunctionTable::Register("set_component", 3, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args) //component_name, entity, data (hash_map)
            {
                if (args.size() < 3) return ELScript::Value();

                return SetComponentELS(args[0].strVal, (int)args[1].numberVal, args[2], id);
            });

        ELScript::FunctionTable::Register("is_action_pressed", 1, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args)  //key
            {
                if (args.size() < 1) return false;
                auto result = Core::ServiceLocator::Get<Input::InputSystem>()->IsActionPressed(args[0].strVal);
                return result;
            });
        ELScript::FunctionTable::Register("get_mouse_delta",0, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args)
            {
                auto result = Core::ServiceLocator::Get<Input::InputSystem>()->GetMouseDelta().ToArray();

                auto x = ELScript::Value(result[0]);
                auto y = ELScript::Value(result[1]);
                auto r = std::make_shared<std::vector<ELScript::Value>>();
                r->push_back(x);
                r->push_back(y);
                return r;
            });
        ELScript::FunctionTable::Register("rotate", 3, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args) //entity axis value
            {
                if (args.size() < 3) return 0;
                auto* tr = m_eSpace->GetServiceLocator().GetByService<Components::TransformComponentService>();
                       
                auto axis = args[1];
                if (!axis.arrayVal || axis.arrayVal->size() < 3) return 0;
                Math::Vector3 axisv(axis.arrayVal->at(0).numberVal, axis.arrayVal->at(1).numberVal, axis.arrayVal->at(2).numberVal);
                
                tr->RotateLocal(args[0].numberVal, axisv, args[2].numberVal);

                return 0;
            });
        ELScript::FunctionTable::Register("translate", 2, [&](ELScript::ECID id, const std::vector<ELScript::Value>& args) //entity axis
            {
                if (args.size() < 2) return 0;
                auto* tr = m_eSpace->GetServiceLocator().GetByService<Components::TransformComponentService>();

                auto axis = args[1];
                if (!axis.arrayVal || axis.arrayVal->size() < 3) return 0;
                Math::Vector3 axisv(axis.arrayVal->at(0).numberVal, axis.arrayVal->at(1).numberVal, axis.arrayVal->at(2).numberVal);

                tr->TranslateLocal(args[0].numberVal, axisv);

                return 0;
            });
    }

    void ELScriptComponentService::InvokeProcess(Scenes::SceneContext& context, float delta)
    {
        ECS::View<ELScriptComponent> view(context.GetEntitySpace().GetRegistry());

        view.Each([&](ECS::EntityID id, ELScriptComponent& script)
            {
                ELScript::Interpreter::Get().CallFunction(script.scriptID, "process", { ELScript::Value(delta) });
            });
    }

    void ELScriptComponentService::InvokePhysicsProcess(Scenes::SceneContext& context, float delta)
    {
        ECS::View<ELScriptComponent> view(context.GetEntitySpace().GetRegistry());

        view.Each([&](ECS::EntityID id, ELScriptComponent& script)
            {
                ELScript::Interpreter::Get().CallFunction(script.scriptID, "physics_process", { ELScript::Value(delta) });
            });
    }

    void ELScriptComponentService::AttachingAPIMethods(ELScriptComponent& script)
    {

    }

    ELScript::Value ELScriptComponentService::DCL_TO_ELS(DCL::Value field) {
        if (field.type == DCL::ValueType::NUMBER)
        {
            return(float)field.numberVal;
        }
        if (field.type == DCL::ValueType::STRING)
        {
            return field.strVal;
        }
        if (field.type == DCL::ValueType::BOOL)
        {
            return field.boolVal;
        }
        if (field.type == DCL::ValueType::ARRAY)
        {
            ELScript::Value value = std::make_shared<std::vector<ELScript::Value>>();

            for (auto element : *field.arrayVal)
            {
                value.arrayVal->push_back(DCL_TO_ELS(element));
            }

            return value;
        }
        return 0;
    }

    DCL::Value ELScriptComponentService::ELS_TO_DCL(ELScript::Value field)
    {
        if (field.type == ELScript::NUMBER)
        {
            return(float)field.numberVal;
        }
        if (field.type == ELScript::STRING)
        {
            return field.strVal;
        }
        if (field.type == ELScript::BOOL)
        {
            return field.boolVal;
        }
        if (field.type == ELScript::ARRAY)
        {
            DCL::Value value = std::make_shared<std::vector<DCL::Value>>();

            for (auto element : *field.arrayVal) 
            {
                value.arrayVal->push_back(ELS_TO_DCL(element));
            }

            return value;
        }
        return 0;
    }

    ELScript::Value ELScriptComponentService::GetComponentELS(std::string component_name, ECS::EntityID entity, ELScript::ECID scriptID)
    {
        auto& csl = m_eSpace->GetServiceLocator();
        auto* service = static_cast<ELComponentService*>(csl.GetByTypeString(component_name));

        if (!service)
        {
            // посылаем сообщение об ошибке в elscript или просто возвращаем нулевой результат
            return ELScript::Value();
        }
        void* component = m_eSpace->GetRegistry().GetByTypeIndex(service->GetComponentType(), entity);
        if (!component)
        {
            // посылаем сообщение об ошибке в elscript или просто возвращаем нулевой результат
            return ELScript::Value();
        }

        ELScript::Value result(std::make_shared<std::unordered_map<std::string, ELScript::Value>>());

        auto dcl_map = service->GetComponentFields(entity);

        for (auto& ppair : dcl_map->ordered_fields)
        {
            if (!ppair.isContainer)(*result.dictVal)[ppair.name] = DCL_TO_ELS(ppair.value);
        }
        return result;
    }

    ELScript::Value ELScriptComponentService::SetComponentELS(std::string component_name, ECS::EntityID entity, ELScript::Value data, ELScript::ECID scriptID)
    {
        auto& csl = m_eSpace->GetServiceLocator();
        auto* service = static_cast<ELComponentService*>(csl.GetByTypeString(component_name));

        if (!service)
        {
            // посылаем сообщение об ошибке в elscript или просто возвращаем нулевой результат
            return false;
        }

        auto esl_map = data.dictVal;
        for (auto& ppair : *esl_map)
        {
            service->SetComponentField(entity, ppair.first, ELS_TO_DCL(ppair.second)) ;
        }
        return true;
    }
}