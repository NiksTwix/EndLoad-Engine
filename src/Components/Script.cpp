#include <Components\Script.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <Services\Events\EventManager.hpp>
#include <Core\ServiceLocator.hpp>

ScriptService::ScriptService()
{
    m_stringType = "Script";
    m_ComponentType = typeid(Script);
}

void ScriptService::InvokeProcess(SceneContext& context, float delta)
{
    ECS::View<Script> view(context.GetEntitySpace().GetRegistry());

    view.Each([&](ECS::EntityID id, Script& script)
        {
            script.script->Process(delta);
        });
}

void ScriptService::InvokePhysicsProcess(SceneContext& context, float delta)
{
    ECS::View<Script> view(context.GetEntitySpace().GetRegistry());

    view.Each([&](ECS::EntityID id, Script& script)
        {
            script.script->PhysicsProcess(delta);
        });
}

void ScriptService::RemoveScript(ECS::EntityID id, SceneContext& context)
{
    std::lock_guard lock(m_mutex);
    if (!context.GetEntitySpace().HasComponent<Script>(id)) return;
    auto& script = context.GetEntitySpace().GetComponent<Script>(id);
    script.script->Destroyed(); // Уведомляем о разрушении
    context.GetEntitySpace().RemoveComponent<Script>(id);
    
}

void ScriptService::Init()
{
}

void ScriptService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}

void ScriptService::Shutdown()
{
}

std::unordered_map<std::string, ECS::ESDL::ESDLType> ScriptService::GetComponentFields(void* component)
{
    return std::unordered_map<std::string, ECS::ESDL::ESDLType>();
}

void ScriptService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
}
