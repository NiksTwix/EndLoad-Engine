#include <queue>
#include <Components/UI/UIElementComponent.hpp>
#include <Components/Basic/CameraComponent.hpp>
#include <Components\ComponentTags.hpp>

void UIElementService::Init()
{
}

void UIElementService::Update(ECS::EntitySpace* eSpace)
{
	m_eSpace = eSpace;
}

void UIElementService::Shutdown()
{
}



void UIElementService::UpdateCameraID()
{
    if (m_eSpace == nullptr) return;

    ECS::Registry& registry = m_eSpace->GetRegistry();
    ECS::HierarchyTree& tree = m_eSpace->GetTree();
    ECS::ComponentServiceLocator& csl = m_eSpace->GetServiceLocator();

    auto camera = csl.GetByService<CameraService>()->GetRenderCameraID();

    std::queue<ECS::EntityID> processQueue;
    processQueue.push(camera);
    while (!processQueue.empty()) 
    {
        auto current = processQueue.front();
        processQueue.pop();

        for (auto child : tree.GetChildren(current)) {
            if (registry.Has<UIElementComponent>(child) && !registry.Has<CameraComponent>(child))
            {
                registry.Get<UIElementComponent>(child).CameraID = camera;
                processQueue.push(child);
            }
        }
    }
    
}


void UIElementService::UpdateUIGT() //UI global transform
{
    if (m_eSpace == nullptr) return;

    ECS::Registry& registry = m_eSpace->GetRegistry();
    ECS::HierarchyTree& tree = m_eSpace->GetTree();
    ECS::ComponentServiceLocator& csl = m_eSpace->GetServiceLocator();

    auto cameraID = csl.GetByService<CameraService>()->GetRenderCameraID();

    ECS::EntityID current = cameraID;
    LocalTransformComponent currentTransform;
    std::queue<ECS::EntityID> processQueue;
    //Предподготовка
    for (auto child : tree.GetChildren(current))
    {
        auto* t = registry.TryGet<LocalTransformComponent>(child);
        if (registry.Has<UIElementComponent>(child) && t != nullptr)
        {
            GlobalTransformComponent gt;
            gt.RotationQuat = t->RotationQuat;
            gt.Translation = t->Translation;
            gt.Scale = t->Scale;
            gt.Visible = t->Visible;
            gt.ModelMatrix = csl.GetByService<TransformService>()->GetModelMatrix(gt);
            registry.Replace<GlobalTransformComponent>(child, gt);
            processQueue.push(child);
        }
    }
    //Основной цикл
    while (!processQueue.empty())
    {
        current = processQueue.front();
        processQueue.pop();

        auto parent = registry.Get<GlobalTransformComponent>(current);

        for (auto child : tree.GetChildren(current)) 
        {
            auto* t = registry.TryGet<LocalTransformComponent>(child);
           
            if (registry.Has<UIElementComponent>(child) && t != nullptr)
            {
                GlobalTransformComponent gt;
                gt.RotationQuat = Math::Normalize(t->RotationQuat * parent.RotationQuat); // Умножение кватернионов! Старое на новое
                gt.Translation = parent.Translation + (parent.RotationQuat * t->Translation) * parent.Scale;
                gt.Scale = t->Scale * parent.Scale;
                gt.Visible = parent.Visible && t->Visible;
                gt.ModelMatrix = csl.GetByService<TransformService>()->GetModelMatrix(gt);
                registry.Replace<GlobalTransformComponent>(child, gt);
                processQueue.push(child);
            }
        }
    }
}

void UIElementService::UpdateComponent(ECS::EntityID entity)
{
    if (m_eSpace == nullptr || entity == ECS::INVALID_ID) return;
    UIElementComponent& component = m_eSpace->GetRegistry().Get<UIElementComponent>(entity);

    //Обработка компонента
}
std::unordered_map<std::string, ECS::ESDL::ESDLType> UIElementService::GetComponentFields(void* component)
{
    return std::unordered_map<std::string, ECS::ESDL::ESDLType>();
}

void UIElementService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
}
