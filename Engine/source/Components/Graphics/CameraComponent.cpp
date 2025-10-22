#include <Components/Graphics/CameraComponent.hpp>
#include <Components/Graphics/MeshComponent.hpp>
#include <Services\Scenes\SceneContext.hpp>
namespace Components 
{
    CameraComponentService::CameraComponentService()
    {
        m_stringType = "Camera";
        m_ComponentType = typeid(CameraComponent);
    }

    CameraComponent CameraComponentService::CreateCamera(int width, int height, ProjectionType projection_type, float fov, float ortho_size)
    {

        CameraComponent cam;

        cam.width = width;
        cam.height = height;
        cam.orthoSize = ortho_size;
        UpdateProjectionMatrix(cam, width, height, projection_type, fov);

        cam.projectionType = projection_type;
        return cam;
    }


    void CameraComponentService::UpdateProjectionMatrix(CameraComponent& cam, int width, int height, ProjectionType type, float fov) {
        float aspect = (float)width / (float)height;
        cam.fov = fov;
        switch (type) {
        case ProjectionType::Orthogonal:
            cam.projection = Math::Ortho(
                -cam.orthoSize * aspect,
                cam.orthoSize * aspect,
                -cam.orthoSize,
                cam.orthoSize,
                cam.near,
                cam.far
            );
            break;

        case ProjectionType::Perspective:
            cam.projection = Math::Perspective(
                Math::EulerToRadians(cam.fov),
                aspect,
                cam.near,
                cam.far
            );
            break;
        }
        cam.projectionType = type;
        cam.isDirty = true;
    }

    void CameraComponentService::UpdateViewMatrix(CameraComponent& cam, GlobalTransformComponent& transform)
    {
        cam.view = Math::LookAt(transform.origin, transform.origin + cam.front, cam.up);//Math::FromGLM(glm::lookAt(Math::ToGLM(transform.Translation), Math::ToGLM(transform.Translation + cam.Front), Math::ToGLM(cam.Up)));// 
    }

    Math::Matrix4x4 CameraComponentService::GetCameraMatrix(CameraComponent& cam)
    {
        return cam.projection * cam.view;
    }

    void CameraComponentService::UpdateCameraVectors(CameraComponent& cam, GlobalTransformComponent& transform)
    {
        // 1. Гарантируем валидность кватерниона
        if (Math::IsNaN(transform.basis.GetRotation())) {
            transform.basis.SetRotation(Math::Identity());
        }

        cam.front = Math::Normalize(transform.basis.GetRotation() * Math::Vector3::Forward()); //Math::Vector3::Forward() = 0,0,-1

        cam.right = Math::Normalize(Math::Cross(cam.front, cam.worldUp));
        cam.up = Math::Normalize(Math::Cross(cam.right, cam.front));

        cam.isDirty = true;
    }

    void CameraComponentService::UpdateFrustumCulling(CameraComponent& cam)
    {
        Math::Matrix4x4 VP = cam.projection * cam.view;
        // Left
        cam.frustumPlanes[0] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.x0, VP.x1, VP.x2, VP.x3);
        // Right
        cam.frustumPlanes[1] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.x0, VP.x1, VP.x2, VP.x3);
        // Bottom
        cam.frustumPlanes[2] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.y0, VP.y1, VP.y2, VP.y3);
        // Top
        cam.frustumPlanes[3] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.y0, VP.y1, VP.y2, VP.y3);
        // Near
        cam.frustumPlanes[4] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) + Math::Vector4(VP.z0, VP.z1, VP.z2, VP.z3);
        // Far
        cam.frustumPlanes[5] = Math::Vector4(VP.w0, VP.w1, VP.w2, VP.w3) - Math::Vector4(VP.z0, VP.z1, VP.z2, VP.z3);

        for (auto& plane : cam.frustumPlanes)
        {
            float len = Math::Length(plane);
            plane /= len;
        }
    }

    void CameraComponentService::UpdateCameraData(Scenes::SceneContext* context, ECS::EntityID entity)
    {
        if (!context) return;
        auto& camera = context->GetEntitySpace().GetComponent<CameraComponent>(entity);
        auto& global_transform = context->GetEntitySpace().GetComponent<GlobalTransformComponent>(entity);

        UpdateCameraVectors(camera, global_transform);
        if (camera.isDirty) {
            UpdateViewMatrix(camera, global_transform);
            UpdateFrustumCulling(camera);
            camera.isDirty = false;
        }
    }


    bool CameraComponentService::IsVisibleMesh(const Components::MeshComponent& mesh, const CameraComponent& camera)
    {
        for (auto& plane : camera.frustumPlanes) {
            Math::Vector3 positive = mesh.aabbWorld.Min;
            if (plane.x >= 0) positive.x = mesh.aabbWorld.Max.x;
            if (plane.y >= 0) positive.y = mesh.aabbWorld.Max.y;
            if (plane.z >= 0) positive.z = mesh.aabbWorld.Max.z;
    
            Math::Vector4 t = plane;
    
            if (Math::Dot((Math::Vector3)t, positive) + plane.w < 0) {
                return false; // Вне пирамиды
            }
        }
        return true;
    }
}