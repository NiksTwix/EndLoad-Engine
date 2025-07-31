#include <Components\Basic\CameraComponent.hpp>
#include <Core\ServiceLocator.hpp>
#include <Services\Serialization\SerializationService.hpp>
#include <Services\Scene\SceneContext.hpp>
#include <ECS\ESDL\ESDLValueParser.hpp>
#include <Services/Utility/Functions.hpp>

CameraService::CameraService()
{
    m_stringType = "Camera";
    m_ComponentType = typeid(CameraComponent);
}

void CameraService::Init()
{
    
}

void CameraService::Shutdown()
{
}

void CameraService::Update(ECS::EntitySpace* eSpace)
{
    m_eSpace = eSpace;
}



CameraComponent CameraService::CreateCamera(int width, int height, ProjectionType projection_type, float fov, float ortho_size)
{
    
    CameraComponent cam;

    cam.Width = width;
    cam.Height = height;
    cam.OrthoSize = ortho_size;
    UpdateProjectionMatrix(cam, width, height, projection_type, fov);

    cam.Projection_Type = projection_type;
    return cam;
}


void CameraService::UpdateProjectionMatrix(CameraComponent& cam, int width, int height, ProjectionType type,float fov) {
    float aspect = (float)width / (float)height;
    cam.FOV = fov;
    switch (type) {
    case ProjectionType::Orthogonal:
        cam.Projection = Math::Ortho(
            -cam.OrthoSize * aspect,
            cam.OrthoSize * aspect,
            -cam.OrthoSize,
            cam.OrthoSize,
            cam.Near,
            cam.Far
        );
        break;

    case ProjectionType::Perspective:
        cam.Projection = Math::Perspective(
            Math::EulerToRadians(cam.FOV),
            aspect,
            cam.Near,
            cam.Far
        );
        break;
    }
    cam.Projection_Type = type;
    cam.isDirty = true;
}

void CameraService::UpdateViewMatrix(CameraComponent& cam, GlobalTransformComponent& transform)
{
    cam.View = Math::LookAt(transform.Translation, transform.Translation + cam.Front, cam.Up);//Math::FromGLM(glm::lookAt(Math::ToGLM(transform.Translation), Math::ToGLM(transform.Translation + cam.Front), Math::ToGLM(cam.Up)));// 
}

Math::Matrix4x4 CameraService::GetCameraMatrix(CameraComponent& cam)
{
    return cam.Projection*cam.View;
}

void CameraService::UpdateCameraVectors(CameraComponent& cam, GlobalTransformComponent& transform)
{
    // 1. Гарантируем валидность кватерниона
    if (Math::IsNaN(transform.RotationQuat)) {
        transform.RotationQuat = Math::Identity();
    }

    cam.Front = Math::Normalize(transform.RotationQuat * Math::Vector3::Forward()); //Math::Vector3::Forward() = 0,0,-1

    cam.Right = Math::Normalize(Math::Cross(cam.Front, cam.WorldUp));
    cam.Up = Math::Normalize(Math::Cross(cam.Right, cam.Front));

    cam.isDirty = true;
}

void CameraService::UpdateFrustumCulling(CameraComponent& cam)
{
    Math::Matrix4x4 VP = cam.Projection * cam.View;
    // Left
    cam.FrustumPlanes[0] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.x0, VP.x1, VP.x2, VP.x3);
    // Right
    cam.FrustumPlanes[1] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.x0, VP.x1, VP.x2, VP.x3);
    // Bottom
    cam.FrustumPlanes[2] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.y0, VP.y1, VP.y2, VP.y3);
    // Top
    cam.FrustumPlanes[3] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.y0, VP.y1, VP.y2, VP.y3);
    // Near
    cam.FrustumPlanes[4] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.z0, VP.z1, VP.z2, VP.z3);
    // Far
    cam.FrustumPlanes[5] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.z0, VP.z1, VP.z2, VP.z3);

    for (auto& plane : cam.FrustumPlanes) 
    {
        float len = Math::Length(plane);
        plane /= len;
    }
}

void CameraService::UpdateAllCameras(SceneContext& context)
{
    std::vector<LocalTransformComponent*> transforms;
    std::vector<CameraComponent*> cameras;
    std::vector<ECS::EntityID> entities;

    ECS::View<LocalTransformComponent, CameraComponent> view(context.GetEntitySpace().GetRegistry());

    view.Each([&](ECS::EntityID id, auto& transform, auto& camera)
        {
            transforms.push_back(&transform);
            cameras.push_back(&camera);
            entities.push_back(id);
        });

    if (transforms.size() == 0) return;

    TransformService* TS = static_cast<TransformService*>(m_eSpace->GetServiceLocator().GetByComponent<LocalTransformComponent>());
    int index = 0;

    for (auto cam_ptr : cameras) 
    {
        auto global_transform = context.GetEntitySpace().GetComponent<GlobalTransformComponent>(entities[index]);

        UpdateCameraVectors(*cam_ptr, global_transform);
        if (cam_ptr->IsCurrent) CurrentCamera = *cam_ptr;
        if (cam_ptr->isDirty) {
            UpdateViewMatrix(*cam_ptr, global_transform);
            UpdateFrustumCulling(*cam_ptr);
            cam_ptr->isDirty = false;
        }
        index++;
    }
}



CameraComponent& CameraService::GetCurrent()
{
    return CurrentCamera;
}

void CameraService::SetAsCurrent(CameraComponent& cam)
{
	cam.IsCurrent = true;
	CurrentCamera = cam;
}

bool CameraService::IsVisibleAABB(const AABB& aabb, const CameraComponent& camera)
{
    for (auto& plane : camera.FrustumPlanes) {
        Math::Vector3 positive = aabb.Min;
        if (plane.x >= 0) positive.x = aabb.Max.x;
        if (plane.y >= 0) positive.y = aabb.Max.y;
        if (plane.z >= 0) positive.z = aabb.Max.z;

        Math::Vector4 t = plane;

        if (Math::Dot((Math::Vector3)t, positive) + plane.w < 0) {
            return false; // Вне пирамиды
        }
    }

    return true;
}



std::unordered_map<std::string, ECS::ESDL::ESDLType> CameraService::GetComponentFields(void* component)
{
    if (!component) return std::unordered_map<std::string, ECS::ESDL::ESDLType>();
    std::unordered_map<std::string, ECS::ESDL::ESDLType> result;
    CameraComponent* camera = static_cast<CameraComponent*>(component);

    result["FOV"] = camera->FOV;
    result["Near"] = camera->Near;
    result["Far"] = camera->Far;
    if (camera->Projection_Type == ProjectionType::Orthogonal) 
    {
        result["Projection"] = "Orthogonal";
    }
    else 
    {
        result["Projection"] = "Perspective";
    }
    result["OrthoSize"] = camera->OrthoSize;
    result["IsCurrent"] = camera->IsCurrent;
    result["Width"] = camera->Width;
    result["Height"] = camera->Height;
    result["WorldUp"] = camera->WorldUp.ToArray();

    return result;
}

void CameraService::AddComponentByFields(ECS::EntityID id, std::unordered_map<std::string, ECS::ESDL::ESDLType> fields)
{
    CameraComponent cam;

    for (auto [fieldName, fieldValue] : fields)
    {
        if (fieldName == "FOV") {
            ECS::ESDL::TryConvertField<float>(cam.FOV, fieldValue);
        }
        else if (fieldName == "Near") {
            ECS::ESDL::TryConvertField<float>(cam.Near, fieldValue);
        }
        else if (fieldName == "Far") {
            ECS::ESDL::TryConvertField<float>(cam.Far, fieldValue);
        }
        else if (fieldName == "OrthoSize") {
            ECS::ESDL::TryConvertField<float>(cam.OrthoSize, fieldValue);
        }
        else if (fieldName == "IsCurrent") {
            ECS::ESDL::TryConvertField<bool>(cam.IsCurrent, fieldValue);
        }
        else if (fieldName == "Width") {
            ECS::ESDL::TryConvertField<int>(cam.Width, fieldValue);
        }
        else if (fieldName == "Height") {
            ECS::ESDL::TryConvertField<int>(cam.Height, fieldValue);
        }
        else if (fieldName == "WorldUp") {
            TrySet3DVector(cam.WorldUp, fieldValue);
        }
        else if (fieldName == "Projection") {
            if (auto wu = ECS::ESDL::GetAs<std::string>(fieldValue)) {
                auto str = *wu;
                if (str == "Orthogonal") cam.Projection_Type = ProjectionType::Orthogonal;
                else cam.Projection_Type = ProjectionType::Perspective;
            }
            else {
                ServiceLocator::Get<Logger>()->Log("(CameraService): camera's deserialization error: value's type is not string. Entity " + std::to_string(id) + ".", Logger::Level::Error);
            }
        }
    }

    if (m_eSpace != nullptr) m_eSpace->GetRegistry().Add<CameraComponent>(id, cam);
    else 
    {
        ServiceLocator::Get<Logger>()->Log("(CameraService): camera's deserialization error: m_eSpace is nullptr.", Logger::Level::Error);
    }
}



