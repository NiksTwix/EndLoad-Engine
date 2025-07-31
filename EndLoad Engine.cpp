// EndLoad Engine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
//#include "Engine/include/Core/ELMath.hpp"
#include <iostream>
#include <EngineMUHF.hpp>
#include <CameraScript.h>
#include <Math\MathFunctions.hpp>
#include <ECS\ESDL\ESDLParser.hpp>


int main()
{
    Core& core = Core::Get();
    auto window = ServiceLocator::Get<WindowManager>()->CreateWindow(1920, 1080, "Test");
    auto context = ServiceLocator::Get<SceneLoader>()->LoadELScene("E:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\TestScene.elscene");
    ServiceLocator::Get<Render>()->AttachSceneToWindow(window->GetID(), context->GetID());
    ScriptComponent scr = context->GetEntitySpace().GetServiceLocator().GetByService<ScriptService>()->Create<CameraScript>(1, context);
    context->GetEntitySpace().AddComponent<ScriptComponent>(1, scr);
    //context->GetEntitySpace().RemoveComponent<ScriptComponent>(1);
    
    size_t last = 2;
    for (int i = 10; i < 10000; i++)
    {
        auto id = context->GetEntitySpace().CreateEntity();
        context->GetEntitySpace().GetTree().AddEntityToEntity(last, id);
        LocalTransformComponent comp;
        MeshComponent mesh = context->GetEntitySpace().GetServiceLocator().GetByService<MeshService>()->CreateMesh("E:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\Untitled.obj");
        MaterialComponent mat = context->GetEntitySpace().GetServiceLocator().GetByService<MaterialService>()->CreateMaterial("E:\\EndLoadEngine\\EndLoad Engine\\TestAssets\\StandartShader.elsh");
        comp.Translation.x = 0;
        comp.Translation.y = 10 * i * 0.001f;
        comp.Translation.z = -10 * i * 0.002f;
        
        context->GetEntitySpace().AddComponent<LocalTransformComponent>(id, comp);
        context->GetEntitySpace().AddComponent<MeshComponent>(id, mesh);
        context->GetEntitySpace().AddComponent<MaterialComponent>(id, mat);
        last = id;
    }
    //size_t last = 3;
    //for (int i = 0; i < 10000; i++)
    //{
    //    auto id = context->GetEntitySpace().CreateEntity();
    //    context->GetEntitySpace().GetTree().AddEntityToEntity(last, id);
    //    LocalTransformComponent comp;
    //    UIElementComponent mesh;
    //    mesh.Size = Math::Vector2(10);
    //    mesh.Color = Math::Vector4(0.5f * std::sin(i), 0.6f * std::cos(i), 0.4f * std::tan(i), 1.0f);
    //    int t = 900;
    //    int t1 = 500;
    //    if (last != 1) { t = 0; t1 = 0; }
    //    comp.Translation.x = t  + 0.1 * i * 0.01f;
    //    comp.Translation.y = t1  + 0.1 * i * 0.01f;
    //    context->GetEntitySpace().AddComponent<LocalTransformComponent>(id, comp);
    //    context->GetEntitySpace().AddComponent<UIElementComponent>(id, mesh);
    //    //context->GetEntitySpace().AddComponent<Tag_NotUpdatableGT>(id, Tag_NotUpdatableGT());
    //    context->GetEntitySpace().GetServiceLocator().GetByService<TransformService>()->RotateLocal(id, Math::Vector3(0, 0, 1), 0.1);
    //    last = id;
    //}

    //auto id = context->GetEntitySpace().CreateEntity();
    //context->GetEntitySpace().GetTree().AddEntityToEntity(last, id);
    //LocalTransformComponent comp;
    //UIElementComponent mesh;
    //mesh.Size = Math::Vector2(400,200);
    //mesh.Color = Math::Vector4(0.5f * std::sin(id), 0.6f * std::cos(id), 0.4f * std::tan(id), 0.5f);
    //comp.Translation.x = 400;
    //comp.Translation.y = 200;
    //context->GetEntitySpace().AddComponent<LocalTransformComponent>(id, comp);
    //context->GetEntitySpace().AddComponent<UIElementComponent>(id, mesh);

    //TextRenderComponent trc;
    //trc.Font = ServiceLocator::Get<ResourceManager>()->Load<FontResource>("C:\\Windows\\Fonts\\Arial.ttf");
    //trc.Text = "Testp";
    //trc.XAlignment = StringAlignment::Far;
    //trc.YAlignment = StringAlignment::Far;
    //trc.Color = Math::Vector4(0, 0, 0, 1.0F);
    //trc.RenderScale = 2.0f;
    //context->GetEntitySpace().AddComponent<TextRenderComponent>(id, trc);
    core.Start();
    core.Shutdown();
    
    //std::type_index t = typeid(TransformComponent);
    //std::cout << sizeof(std::unordered_map<ECS::EntityID,std::vector<std::type_index>>);

    //BeanchMark(10);
}






